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

#ifndef VOLUMEBRIDGE_H
#define VOLUMEBRIDGE_H

#include "mpdclient.h"
#include "jackclient.h"

#include <vector>
#include <string>

class CVolumeBridge
{
  public:
    CVolumeBridge();
    ~CVolumeBridge();

    bool                     Setup(int argc, char *argv[]);
    void                     Run();

  private:
    void                     PrintHelpMessage();

    void                     ConnectJackClient();
    void                     ConnectMpdClient();

    std::string              m_address;
    int                      m_port;

    CJackClient              m_jackclient;
    CMpdClient               m_mpdclient;

    std::string              m_jackname;
    std::vector<std::string> m_portnames;
};

#endif //VOLUMEBRIDGE_H
