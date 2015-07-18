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

#include "LinuxServer.h"
#include "interface/FrontendManager.h"
#include "interface/IGame.h"
#include "interface/network/NetworkFrontend.h"
#include "log/Log.h"

#include <assert.h>
#include <cstring>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <time.h>

using namespace NETPLAY;
using namespace PLATFORM;

#define LISTEN_PORT   34890

CLinuxServer::CLinuxServer(IGame* game, CFrontendManager* callbacks) :
  m_game(game),
  m_callbacks(callbacks),
  m_socketFd(-1)
{
  assert(m_game);
  assert(m_callbacks);
}

bool CLinuxServer::Initialize(void)
{
  uint16_t port = LISTEN_PORT;

  m_socketFd = socket(AF_INET, SOCK_STREAM, 0);
  if (m_socketFd == -1)
    return false;

  fcntl(m_socketFd, F_SETFD, fcntl(m_socketFd, F_GETFD) | FD_CLOEXEC);

  int one = 1;
  setsockopt(m_socketFd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));

  struct sockaddr_in s;
  std::memset(&s, 0, sizeof(s));
  s.sin_family = AF_INET;
  s.sin_port = htons(port);

  int x = bind(m_socketFd, (struct sockaddr *)&s, sizeof(s));
  if (x < 0)
  {
    esyslog("%s - Unable to start server, port already in use?", __FUNCTION__);

    close(m_socketFd);
    m_socketFd = -1;
    return false;
  }

  listen(m_socketFd, 10);

  if (CreateThread())
  {
    isyslog("Netplay server started on port %d", port);
    return true;
  }

  return false;
}

void CLinuxServer::Deinitialize(void)
{
  isyslog("Netplay server shutting down");
  StopThread();
}

void* CLinuxServer::Process(void)
{
  fd_set fds;
  struct timeval tv;

  while (!IsStopped())
  {
    FD_ZERO(&fds);
    FD_SET(m_socketFd, &fds);

    tv.tv_sec = 0;
    tv.tv_usec = 250 * 1000; // 250 ms

    int r = select(m_socketFd + 1, &fds, NULL, NULL, &tv);
    if (r == -1)
    {
      // Failed
    }
    else if (r == 0)
    {
      PLATFORM::CLockObject lock(m_clientMutex);

      // Remove disconnected clients
      /* TODO
      for (std::vector<IFrontend*>::iterator it = m_clients.begin(); it != m_clients.end(); )
      {
        if (!(*it)->IsOpen())
        {
          isyslog("Client seems to be disconnected, removing client");
          delete (*it);
          it = m_clients.erase(it);
        }
        else
        {
          it++;
        }
      }
      */
    }
    else
    {
      int fd = accept(m_socketFd, 0, 0);
      if (fd >= 0)
      {
        NewClientConnected(fd);
      }
      else
      {
        esyslog("%s - accept failed", __FUNCTION__);
      }
    }
  }

  return NULL;
}

void CLinuxServer::NewClientConnected(int fd)
{
  IFrontend* frontend = new CNetworkFrontend(fd);
  if (frontend->Initialize())
  {
    PLATFORM::CLockObject lock(m_clientMutex);
    m_clients.push_back(frontend);
  }
}
