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

#include "interface/FrontendManager.h"

#include "platform/threads/mutex.h"
#include "platform/threads/threads.h"

#include <vector>

namespace NETPLAY
{
  class CConnection;
  class IGame;

  class CServer : protected PLATFORM::CThread
  {
  public:
    CServer(IGame* game, CFrontendManager* callbacks);
    virtual ~CServer(void) { }

    bool Initialize(void);
    void Deinitialize(void);

    void RegisterFrontend(IFrontend* frontend) { m_callbacks->RegisterFrontend(frontend); }
    void UnregisterFrontend(IFrontend* frontend) { m_callbacks->UnregisterFrontend(frontend); }

    void WaitForExit(void) { Sleep(0); }

  protected:
    virtual void* Process(void);

  private:
    void NewClientConnected(int fd);

    IGame* const              m_game;
    CFrontendManager* const   m_callbacks;
    int                       m_socketFd;
    std::vector<CConnection*> m_clients;
    PLATFORM::CMutex          m_clientMutex;
  };
}
