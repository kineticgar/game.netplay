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

#include "Server.h"
#include "IServerSocket.h"
#include "NetworkFrontend.h"
#include "SocketFactory.h"
#include "interface/FrontendManager.h"
#include "log/Log.h"

using namespace NETPLAY;
using namespace PLATFORM;

CServer::CServer(IGame* game, CFrontendManager* frontends) :
  m_game(game),
  m_frontends(frontends)
{
}

bool CServer::Initialize(void)
{
  bool bSuccess = false;

  m_socket = CSocketFactory::CreateServerSocket();
  if (m_socket)
    bSuccess = m_socket->Bind();

  if (bSuccess && CreateThread())
  {
    isyslog("Netplay server starting up");
    return true;
  }

  return false;
}

void CServer::Deinitialize(void)
{
  StopThread(0);

  if (m_socket)
  {
    isyslog("Netplay server shutting down");
    m_socket->Shutdown();
    m_socket.reset();
  }
}

void* CServer::Process(void)
{
  while (!IsStopped())
  {
    SocketPtr socket = m_socket->Listen();
    if (socket)
      AddFrontend(socket);

    CLockObject lock(m_dcMutex);

    // Remove disconnected clients
    for (std::vector<IFrontend*>::iterator it = m_disconnectedClients.begin(); it != m_disconnectedClients.end(); ++it)
      RemoveFrontend(*it);

    m_disconnectedClients.clear();
  }

  return NULL;
}

void CServer::Notify(const Observable& obs, const ObservableMessage msg)
{
  switch (msg)
  {
    case ObservableMessageConnectionLost:
    {
      CLockObject lock(m_dcMutex);
      m_disconnectedClients.push_back(reinterpret_cast<IFrontend*>(const_cast<Observable*>(&obs)));
      break;
    }
    default:
      break;
  }
}

bool CServer::AddFrontend(const SocketPtr& socket)
{
  IFrontend* frontend = new CNetworkFrontend(m_game, socket);
  if (frontend->Initialize())
  {
    frontend->RegisterObserver(this);
    m_frontends->RegisterFrontend(frontend);
    return true;
  }

  return false;
}

void CServer::RemoveFrontend(IFrontend* frontend)
{
  if (m_frontends->UnregisterFrontend(frontend))
  {
    frontend->UnregisterObserver(this);
    frontend->Deinitialize();
    delete frontend;
  }
}
