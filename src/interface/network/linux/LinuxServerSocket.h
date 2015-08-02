/*
 *      Copyright (C) 2015 Garrett Brown
 *      Copyright (C) 2015 Team XBMC
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */
#pragma once

#include "interface/network/IServerSocket.h"

#include "platform/threads/mutex.h"
#include "platform/threads/threads.h"

namespace NETPLAY
{
  class CFrontendManager;
  class IFrontend;
  class IGame;

  class CLinuxServerSocket : public IServerSocket
  {
  public:
    CLinuxServerSocket(void);
    virtual ~CLinuxServerSocket(void) { Shutdown(); }

    // implementation of IServerSocket
    virtual bool Bind(void);
    virtual void Shutdown(void);
    virtual SocketPtr Listen(void);

  private:
    int m_socketFd;
  };
}
