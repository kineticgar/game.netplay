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

#include "Socket.h"
#include "log/Log.h"
#include "utils/StringUtils.h"

#include <algorithm>
#include <fcntl.h>
#include <netdb.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace NETPLAY;

CSocket::CSocket(const std::string& strAddress, uint16_t port, SOCKET_TYPE type) :
  m_bServer(!strAddress.empty()),
  m_strAddress(strAddress),
  m_port(port),
  m_type(type),
  m_fd(-1)
{
}

CSocket::CSocket(uint16_t port, SOCKET_TYPE type) :
  m_bServer(true),
  m_port(port),
  m_type(type),
  m_fd(-1)
{
}

bool CSocket::Connect(void)
{
  addrinfo hints = GetHints();

  addrinfo* res = nullptr;

  std::string strPort = StringUtils::Format("%hu", m_port);
  if (getaddrinfo(!m_bServer ? m_strAddress.c_str() : NULL, strPort.c_str(), &hints, &res) < 0)
    return false;

  if (m_type == SOCKET_TYPE_TCP)
    ConnectTCP(res);
  else
    ConnectUDP(res);

  if (res)
    freeaddrinfo(res);

  return m_fd >= 0;
}

void CSocket::ConnectTCP(const addrinfo* res)
{
  // If "localhost" is used, it is important to check every possible address for IPv4/IPv6
  for ( ; res != nullptr; res = res->ai_next)
  {
    int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (fd < 0)
      continue;

    if (m_bServer)
    {
      if (connect(fd, res->ai_addr, res->ai_addrlen) >= 0)
      {
        m_fd = fd;
        break;
      }
    }
    else
    {
      int yes = 1;
      setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

      if (bind(fd, res->ai_addr, res->ai_addrlen) >= 0)
      {
        if (listen(fd, 1) >= 0)
        {
          sockaddr_storage other_addr;
          unsigned int addr_size = sizeof(other_addr);

          int new_fd = accept(fd, reinterpret_cast<sockaddr*>(&other_addr), &addr_size);

          if (new_fd >= 0)
          {
            close(fd);
            m_fd = new_fd;
            break;
          }
        }
      }
    }

    close(fd);
  }
}

void CSocket::ConnectUDP(const addrinfo* res)
{
  int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (fd >= 0)
  {
    if (m_bServer)
    {
      m_fd = fd;
    }
    else
    {
      // Not sure if we have to do this for UDP, but hey :)
      int yes = 1;
      setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

      if (bind(fd, res->ai_addr, res->ai_addrlen) >= 0)
      {
        m_fd = fd;
      }
      else
      {
        esyslog("Failed to bind socket");
        close(fd);
      }
    }
  }
}

void CSocket::Shutdown(void)
{
  if (m_fd >= 0)
  {
#if defined(_WIN32)
    // WinSock has headers from the stone age
    closesocket(m_fd);
#else
    close(m_fd);
#endif
  }
}

bool CSocket::nonblock(void)
{
#if defined(_WIN32)
  u_long mode = 1;
  return ioctlsocket(fd, FIONBIO, &mode) == 0;
#else
  return fcntl(m_fd, F_SETFL, fcntl(m_fd, F_GETFL) | O_NONBLOCK) == 0;
#endif
}

int CSocket::send_all_blocking(const void *data_, size_t size)
{
  const uint8_t *data = (const uint8_t*)data_;

  while (size)
  {
    ssize_t ret = send(m_fd, (const char*)data, size, 0);
    if (ret <= 0)
      return false;

    data += ret;
    size -= ret;
  }

  return true;
}

int CSocket::receive_all_blocking(uint8_t *data, size_t size)
{
  while (size)
  {
    ssize_t ret = recv(m_fd, data, size, 0);
    if (ret <= 0)
      return false;

    data += ret;
    size -= ret;
  }

  return true;
}

bool CSocket::SendTo(const uint8_t* buf, size_t size, int flags, const struct sockaddr* addr, unsigned int addr_len)
{
  return sendto(m_fd, buf, size, flags, addr, addr_len) == static_cast<ssize_t>(size);
}

bool CSocket::ReceiveFrom(uint8_t* buf, size_t size, int flags, struct sockaddr* addr, unsigned int *addr_len)
{
  return recvfrom(m_fd, buf, size, flags, addr, addr_len) == static_cast<ssize_t>(size);
}

CSocket* CSocket::Accept(struct sockaddr* addr, unsigned int* addr_size)
{
  int new_fd = accept(m_fd, addr, addr_size);

  if (new_fd >= 0)
  {
    CSocket* newSocket = new CSocket(m_strAddress, m_port, m_type);
    newSocket->m_fd = new_fd;
    return newSocket;
  }

  return NULL;
}

void CSocket::Set(fd_set* fds) const
{
  FD_SET(m_fd, fds);
}

bool CSocket::IsSet(fd_set* fds) const
{
  return FD_ISSET(m_fd, fds);
}

void CSocket::SetSendBuffer(int bufsize)
{
  setsockopt(m_fd, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(int));
}

int CSocket::max_fd(const CSocket* other_socket) const
{
  return other_socket ? std::max(m_fd, other_socket->m_fd) : m_fd;
}

int CSocket::select_sockets(int nfds, fd_set *readfs, fd_set *writefds, fd_set *errorfds, struct timeval *timeout)
{
  return select(nfds, readfs, writefds, errorfds, timeout);
}

addrinfo CSocket::GetHints(void) const
{
  addrinfo hints = { };

#if defined(_WIN32)
  hints.ai_family = AF_INET;
#else
  hints.ai_family = AF_UNSPEC;
#endif

  hints.ai_socktype = m_type == SOCKET_TYPE_TCP ? SOCK_STREAM : SOCK_DGRAM;

  if (!m_bServer)
    hints.ai_flags = AI_PASSIVE;

   return hints;
}
