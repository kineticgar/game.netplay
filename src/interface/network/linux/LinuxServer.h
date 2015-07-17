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

#include "interface/network/IServer.h"

#include "platform/threads/mutex.h"
#include "platform/threads/threads.h"

#include <vector>

namespace NETPLAY
{
  class CFrontendManager;
  class IFrontend;
  class IGame;

  class CLinuxServer : public IServer,
                       protected PLATFORM::CThread
  {
  public:
    CLinuxServer(IGame* game, CFrontendManager* callbacks);
    virtual ~CLinuxServer(void) { Deinitialize(); }

    // implementation of IServer
    virtual bool Initialize(void);
    virtual void Deinitialize(void);
    virtual void WaitForExit(void) { Sleep(0); }

  protected:
    // implementation of PLATFORM::CThread
    virtual void* Process(void);

  private:
    void NewClientConnected(int fd);

    IGame* const              m_game;
    CFrontendManager* const   m_callbacks;
    int                       m_socketFd;
    std::vector<IFrontend*>   m_clients;
    PLATFORM::CMutex          m_clientMutex;
  };
}
