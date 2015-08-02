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

#include "NetworkTypes.h"
#include "utils/IAbortable.h"
#include "utils/Observer.h"

#include "platform/threads/mutex.h"
#include "platform/threads/threads.h"

#include <vector>

namespace NETPLAY
{
  class CFrontendManager;
  class IFrontend;
  class IGame;

  class CServer : public IAbortable,
                  public Observer,
                  protected PLATFORM::CThread
  {
  public:
    CServer(IGame* game, CFrontendManager* frontends);

    virtual ~CServer(void) { Deinitialize(); }

    /*!
     * \brief Initialize this server instance
     */
    bool Initialize(void);

    /*!
     * \brief Deinitialize this service instance
     * \comment Implementation of IAbortable
     */
    virtual void Deinitialize(void);

    /*!
     * \brief Block until the server has been deinitialized
     */
    void WaitForExit(void) { Sleep(0); }

    // implementation of Observer
    virtual void Notify(const Observable& obs, const ObservableMessage msg);

  protected:
    // implementation of CThead
    virtual void* Process(void);

  private:
    bool AddFrontend(const SocketPtr& socket);
    void RemoveFrontend(IFrontend* frontend);

    IGame* const            m_game;
    CFrontendManager* const m_frontends;
    ServerSocketPtr         m_socket;
    std::vector<IFrontend*> m_disconnectedClients;
    PLATFORM::CMutex        m_dcMutex; // Mutex for m_disconnectedClients
  };
}
