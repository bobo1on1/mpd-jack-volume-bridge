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

#ifndef MPDCLIENT_H
#define MPDCLIENT_H

#include <deque>
#include <string>

#include "tcpsocket.h"

class CMpdClient
{
  public:
    CMpdClient();
    ~CMpdClient();

    void             Setup(std::string address, int port);
    void             Process();
    float            Volume();

  private:
    bool             OpenSocket();
    bool             GetPlayStatus();

    int              m_port;
    std::string      m_address;
    CTcpClientSocket m_socket;
    float            m_volume;
};


#endif //MPDCLIENT_H
