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

#include "LinuxServerSocket.h"
#include "LinuxSocket.h"
#include "log/Log.h"

#include <cstring>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <time.h>

using namespace NETPLAY;
using namespace PLATFORM;

#define LISTEN_PORT   35890

CLinuxServerSocket::CLinuxServerSocket(void) :
  m_socketFd(-1)
{
}

bool CLinuxServerSocket::Bind(void)
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
    esyslog("Unable to start server, port already in use?");
    Shutdown();
    return false;
  }

  listen(m_socketFd, 10);

  return true;
}

void CLinuxServerSocket::Shutdown(void)
{
  if (m_socketFd > -1)
  {
    close(m_socketFd);
    m_socketFd = -1;
  }
}

SocketPtr CLinuxServerSocket::Listen(void)
{
  SocketPtr socket;

  fd_set fds;
  struct timeval tv;

  FD_ZERO(&fds);
  FD_SET(m_socketFd, &fds);

  tv.tv_sec = 0;
  tv.tv_usec = 250 * 1000; // 250 ms

  int r = select(m_socketFd + 1, &fds, NULL, NULL, &tv);
  if (r == -1)
  {
    // Failed
  }
  else if (r != 0)
  {
    int fd = accept(m_socketFd, 0, 0);
    if (fd >= 0)
    {
      socket = SocketPtr(new CLinuxSocket(fd));
    }
    else
    {
      esyslog("%s - accept failed", __FUNCTION__);
    }
  }

  return socket;
}
