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

#include "LinuxConnection.h"
#include "LinuxSocket.h"
#include "log/Log.h"
#include "utils/Observer.h"

#include <cstdio>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <stdint.h>

using namespace NETPLAY;
using namespace PLATFORM;

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

// --- CLinuxConnection --------------------------------------------------------

CLinuxConnection::CLinuxConnection(IGame* game, int fd) :
  CConnection(game),
  m_fd(fd),
  m_socket(new CLinuxSocket)
{
  m_socket->SetHandle(m_fd);
}

CLinuxConnection::~CLinuxConnection(void)
{
  Close();
  delete m_socket;
}

bool CLinuxConnection::Open(void)
{
  struct sockaddr_in sin;
  socklen_t len = sizeof(sin);

  if (getpeername(m_fd, reinterpret_cast<struct sockaddr*>(&sin), &len))
  {
    esyslog("%s - getpeername() failed, dropping new incoming connection", __FUNCTION__);
    close(m_fd);
    return false;
  }

  if (fcntl(m_fd, F_SETFL, fcntl (m_fd, F_GETFL) | O_NONBLOCK) == -1)
  {
    esyslog("%s - Error setting control socket to nonblocking mode", __FUNCTION__);
    return false;
  }

  int val = 1;
  setsockopt(m_fd, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(val));

  val = 30;
  setsockopt(m_fd, SOL_TCP, TCP_KEEPIDLE, &val, sizeof(val));

  val = 15;
  setsockopt(m_fd, SOL_TCP, TCP_KEEPINTVL, &val, sizeof(val));

  val = 5;
  setsockopt(m_fd, SOL_TCP, TCP_KEEPCNT, &val, sizeof(val));

  val = 1;
  setsockopt(m_fd, SOL_TCP, TCP_NODELAY, &val, sizeof(val));

  m_strClientAddress = ip2txt(sin.sin_addr.s_addr, sin.sin_port);

  isyslog("Client connected: %s", m_strClientAddress.c_str());

  return CConnection::Open();
}

void CLinuxConnection::Close(void)
{
  CConnection::Close();

  m_socket->Close();
}

bool CLinuxConnection::ReadData(std::string& buffer, size_t totalBytes, unsigned int timeoutMs)
{
  if (totalBytes == 0)
    return false;

  buffer.resize(totalBytes);
  uint8_t* data = reinterpret_cast<uint8_t*>(const_cast<char*>(buffer.c_str()));

  unsigned int bytesRead = m_socket->Read(data, totalBytes, timeoutMs);

  return bytesRead == totalBytes;
}

bool CLinuxConnection::SendData(const std::string& request)
{
  if (request.empty())
    return false;

  m_socket->Write(reinterpret_cast<const uint8_t*>(request.c_str()), request.size());

  return true;
}
