/*  This file is part of mpd-jack-volume-bridge.

    mpd-jack-volume-bridge is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    mpd-jack-volume-bridge is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with mpd-jack-volume-bridge.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <cstdio>
#include <cstring>
#include <sstream>
#include <thread>
#include <chrono>
#include <algorithm>

#include "mpdclient.h"
#include "misc.h"

using namespace std;

CMpdClient::CMpdClient()
{
  m_port   = 0;
  m_volume = 0.2;
}

CMpdClient::~CMpdClient()
{
}

void CMpdClient::Setup(std::string address, int port)
{
  m_port    = port;
  m_address = address;
}

void CMpdClient::Process()
{
  for(;;)
  {
    if (!m_socket.IsOpen())
    {
      if (!OpenSocket())
        continue;
    }

    if (!GetPlayStatus())
    {
      m_socket.Close();
      std::this_thread::sleep_for(std::chrono::seconds(10));
    }
    else
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
  }

  m_socket.Close();
}

bool CMpdClient::OpenSocket()
{
  m_socket.Close();
  int returnv = m_socket.Open(m_address, m_port, 10000000);

  if (returnv != SUCCESS)
  {
    printf("ERROR: Connecting to %s:%i, %s\n", m_address.c_str(), m_port, m_socket.GetError().c_str());
    m_socket.Close();

    if (returnv != TIMEOUT)
      std::this_thread::sleep_for(std::chrono::seconds(10));

    return false;
  }
  else
  {
    printf("Connected to %s:%i\n", m_address.c_str(), m_port);
    return true;
  }
}

bool CMpdClient::GetPlayStatus()
{
  CTcpData data;
  data.SetData("status\n");
  if (m_socket.Write(data) != SUCCESS)
  {
    printf("ERROR: Writing socket: %s\n", m_socket.GetError().c_str());
    return false;
  }

  data.Clear();
  int volume = -1;
  for(;;)
  {
    if (m_socket.Read(data) != SUCCESS)
    {
      printf("ERROR: Reading socket: %s\n", m_socket.GetError().c_str());
      return false;
    }

    stringstream datastream(data.GetData());
    string line;
    while (1)
    {
      getline(datastream, line);
      if (datastream.fail())
        break;

      string tmpline = line;
      string word;
      if (GetWord(tmpline, word))
      {
        if (word == "volume:")
        {
          int parsevolume;
          if (GetWord(tmpline, word) && StrToInt(word, parsevolume))
            volume = parsevolume;
        }
      }

      if (line == "OK")
      {
        if(volume != -1)
        {
          float newvolume = (float)volume / 100.0f;
          if (newvolume != m_volume)
            m_volume = std::min(1.0f, std::max(0.0f, newvolume));
        }

        return true;
      }
    }
  }

  return false;
}

float CMpdClient::Volume()
{
  return m_volume;
}
