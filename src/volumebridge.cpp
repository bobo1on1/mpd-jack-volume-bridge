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

#include "volumebridge.h"
#include <thread>
#include <chrono>
#include <cstdio>
#include <getopt.h>

CVolumeBridge::CVolumeBridge() : m_jackclient(m_mpdclient)
{
  m_address = "localhost";
  m_port    = 6600;
}

CVolumeBridge::~CVolumeBridge()
{
}

bool CVolumeBridge::Setup(int argc, char *argv[])
{
  struct option longoptions[] =
  {
    {"help",        no_argument,       NULL, 'h'},
    {"name",        required_argument, NULL, 'n'},
    {"port",        required_argument, NULL, 'p'},
    {"mpd",         required_argument, NULL, 'm'},
    {0, 0, 0, 0}
  };

  const char* shortoptions = "hn:p:m:";
  int         optionindex = 0;
  int         c;

  while ((c = getopt_long(argc, argv, shortoptions, longoptions, &optionindex)) != -1)
  {
    if (c == 'h' || c == '?')
    {
      PrintHelpMessage();
      return false;
    }
    else if (c == 'n')
    {
      m_jackname = optarg;
    }
    else if (c == 'p')
    {
      m_portnames.push_back(optarg);
    }
    else if (c == 'm')
    {
      char address[256] = {};
      int  port;

      int retval = sscanf(optarg, "%[^:]:%i", address, &port);
      if (retval == 0)
      {
        printf("ERROR: invalid argument for -m: \"%s\"\n", optarg);
        return 1;
      }

      if (retval >= 1)
        m_address = address;

      if (retval == 2)
        m_port = port;
    }
  }

  //set the default jack name if not set by command line argument
  if (m_jackname.empty())
    m_jackname = "mpdvolume";

  //set default port names if no ports are given
  if (m_portnames.empty())
  {
    m_portnames.push_back("left");
    m_portnames.push_back("right");
  }

  return true;
}

void CVolumeBridge::PrintHelpMessage()
{
  printf("usage: mpd-jack-volume-bridge [option]\n");
  printf("\n");
  printf("Creates jack input-output port pairs, copies audio from input to output ports,\n");
  printf("scales the audio with the volume from mpd.\n");
  printf("\n");
  printf("  options:\n");
  printf("\n");
  printf("    -h, --help:      Print this message.\n");
  printf("\n");
  printf("    -n, --name       Set the name of the jack client to create.\n");
  printf("\n");
  printf("    -p, --port:      Adds a jack input-output port pair.\n");
  printf("                     The input port is suffixed with -in.\n");
  printf("                     The output port is suffixed with -out.\n");
  printf("                     If no ports are added, left and right ports\n");
  printf("                     are added by default.\n");
  printf("\n");
  printf("    -m, --mpd:       Set the address:port to connect to,\n");
  printf("                     default is localhost:6600\n");
  printf("  example:\n");
  printf("    mpd-jack-volume-bridge -p left -p right -m localhost:6600\n");
  printf("\n");
}

void CVolumeBridge::Run()
{
  //start a thread to connect to jackd
  std::thread jackthread(&CVolumeBridge::ConnectJackClient, this);

  //connect to mpd from this thread
  ConnectMpdClient();
}

void CVolumeBridge::ConnectJackClient()
{
  for(;;)
  {
    if (!m_jackclient.Setup(m_jackname, m_portnames))
    {
      printf("ERROR: Setting up jack client failed, retrying in 10 seconds\n");
      m_jackclient.Cleanup();
      std::this_thread::sleep_for(std::chrono::seconds(10));
      continue;
    }

    m_jackclient.Run();

    //prevent busy looping in case Run() fails quickly after Setup() succeeds
    std::this_thread::sleep_for(std::chrono::seconds(1));

    m_jackclient.Cleanup();
  }
}

void CVolumeBridge::ConnectMpdClient()
{
  m_mpdclient.Setup(m_address, m_port);
  m_mpdclient.Process();
}
