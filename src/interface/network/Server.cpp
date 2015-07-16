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
#include "Client.h"
#include "Connection.h"
#include "interface/IGame.h"
#include "log/Log.h"

#include <assert.h>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <time.h>

using namespace NETPLAY;
using namespace PLATFORM;

#define LISTEN_PORT   34890

// --- Version -----------------------------------------------------------------

namespace NETPLAY
{
  struct Version
  {
  public:
    Version(const std::string& strVersion) :
      version_major(0),
      version_minor(0),
      version_point(0)
    {
      version_major = 0; // TODO
      version_minor = 0; // TODO
      version_point = 0; // TODO
    }

    bool operator<(const Version& rhs) const
    {
      if (version_major < rhs.version_major) return true;
      if (version_major > rhs.version_major) return false;

      if (version_minor < rhs.version_minor) return true;
      if (version_minor > rhs.version_minor) return false;

      if (version_point < rhs.version_point) return true;
      if (version_point > rhs.version_point) return false;

      return false;
    }

    bool operator==(const Version& rhs) const
    {
      return version_major == rhs.version_major &&
             version_minor == rhs.version_minor &&
             version_point == rhs.version_point;
    }

    bool operator<=(const Version& rhs) const { return  operator<(rhs) ||  operator==(rhs); }
    bool operator>(const Version& rhs) const  { return !operator<(rhs) && !operator==(rhs); }
    bool operator>=(const Version& rhs) const { return !operator<(rhs); }

    unsigned int version_major;
    unsigned int version_minor;
    unsigned int version_point;
  };
}

// --- ip2txt ------------------------------------------------------------------

namespace NETPLAY
{
  std::string ip2txt(uint32_t ip, unsigned int port)
  {
    // inet_ntoa is not thread-safe (?)
    unsigned int iph = static_cast<unsigned int>(ntohl(ip));
    unsigned int porth = static_cast<unsigned int>(ntohs(port));

    char str[64];

    if (porth == 0)
    {
      std::sprintf(str, "%d.%d.%d.%d", ((iph >> 24) & 0xff),
                                       ((iph >> 16) & 0xff),
                                       ((iph >> 8)  & 0xff),
                                       ((iph)       & 0xff));
    }
    else
    {
      std::sprintf(str, "%u.%u.%u.%u:%u", ((iph >> 24) & 0xff),
                                          ((iph >> 16) & 0xff),
                                          ((iph >> 8)  & 0xff),
                                          ((iph)       & 0xff),
                                          porth);
    }

    return str;
  }
}

// --- CServer -----------------------------------------------------------------

CServer::CServer(IGame* game, CFrontendManager* callbacks) :
  m_game(game),
  m_callbacks(callbacks),
  m_socketFd(-1)
{
  assert(m_game);
  assert(m_callbacks);
}

bool CServer::Initialize(void)
{
  uint16_t port = LISTEN_PORT;

  m_socketFd = socket(AF_INET, SOCK_STREAM, 0);
  if(m_socketFd == -1)
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

void CServer::Deinitialize(void)
{
  isyslog("Netplay server shutting down");
  StopThread();
}

void* CServer::Process(void)
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
      for (std::vector<CConnection*>::iterator it = m_clients.begin(); it != m_clients.end(); )
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

void CServer::NewClientConnected(int fd)
{
  struct sockaddr_in sin;
  socklen_t len = sizeof(sin);

  if (getpeername(fd, reinterpret_cast<struct sockaddr*>(&sin), &len))
  {
    esyslog("%s - getpeername() failed, dropping new incoming connection", __FUNCTION__);
    close(fd);
    return;
  }

  if (fcntl(fd, F_SETFL, fcntl (fd, F_GETFL) | O_NONBLOCK) == -1)
  {
    esyslog("%s - Error setting control socket to nonblocking mode", __FUNCTION__);
    close(fd);
    return;
  }

  int val = 1;
  setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(val));

  val = 30;
  setsockopt(fd, SOL_TCP, TCP_KEEPIDLE, &val, sizeof(val));

  val = 15;
  setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, &val, sizeof(val));

  val = 5;
  setsockopt(fd, SOL_TCP, TCP_KEEPCNT, &val, sizeof(val));

  val = 1;
  setsockopt(fd, SOL_TCP, TCP_NODELAY, &val, sizeof(val));

  CConnection* connection = new CConnection(fd, ip2txt(sin.sin_addr.s_addr, sin.sin_port).c_str());

  isyslog("Client connected: %s", connection->Name().c_str());

  {
    PLATFORM::CLockObject lock(m_clientMutex);
    m_clients.push_back(connection);
  }
}
