/*
 *      Copyright (C) 2013-2015 Garrett Brown
 *      Copyright (C) 2015 Team XBMC
 *      Portions Copyright (C) 2013-2014 Lars Op den Kamp
 *      Portions Copyright (C) 2000, 2003, 2006, 2008, 2013 Klaus Schmidinger
 *      Portions Copyright (C) 2003-2006 Petri Hintukainen
 *      Portions Copyright (C) 2010 Alwin Esch (Team XBMC)
 *      Portions Copyright (C) 2011 Alexander Pipelka
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
 *  along with this Program; see the file COPYING. If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

/*!
 * \brief Socket wrapper classes
 */

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include "LinuxSocket.h"
#include "filesystem/linux/LinuxPoller.h"
#include "log/Log.h"

#include <cstdio>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>

using namespace NETPLAY;
using namespace PLATFORM;

#define WRITE_POLL_TIMEOUT_MS  100 // Timeout for polling fd before write

#ifndef MSG_MORE
  #define MSG_MORE 0
#endif

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

// --- CLinuxSocket ------------------------------------------------------------

CLinuxSocket::CLinuxSocket(int fd) :
  m_fd(fd),
  m_pollerRead(NULL),
  m_pollerWrite(NULL),
  m_bStop(false)
{
}

bool CLinuxSocket::Connect(void)
{
  if (m_fd < 0)
    return false;

  struct sockaddr_in sin;
  socklen_t len = sizeof(sin);

  if (getpeername(m_fd, reinterpret_cast<struct sockaddr*>(&sin), &len))
  {
    esyslog("CLinuxSocket: getpeername() failed, dropping new incoming connection");
    close(m_fd);
    return false;
  }

  if (fcntl(m_fd, F_SETFL, fcntl (m_fd, F_GETFL) | O_NONBLOCK) == -1)
  {
    esyslog("CLinuxSocket: Error setting control socket to nonblocking mode");
    return false;
  }

  int val = 1;
  setsockopt(m_fd, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(val));

  // Not portable
#if defined(TCP_KEEPIDLE)
  val = 30;
  setsockopt(m_fd, IPPROTO_TCP, TCP_KEEPIDLE, &val, sizeof(val));
#endif

  val = 15;
  setsockopt(m_fd, IPPROTO_TCP, TCP_KEEPINTVL, &val, sizeof(val));

  val = 5;
  setsockopt(m_fd, IPPROTO_TCP, TCP_KEEPCNT, &val, sizeof(val));

  val = 1;
  setsockopt(m_fd, IPPROTO_TCP, TCP_NODELAY, &val, sizeof(val));

  m_strAddress = ip2txt(sin.sin_addr.s_addr, sin.sin_port);

  isyslog("Client connected: %s", m_strAddress.c_str());

  CLockObject lock(m_abortMutex);

  m_pollerRead  = new CLinuxPoller(m_fd);
  m_pollerWrite = new CLinuxPoller(m_fd, true);

  return true;
}

void CLinuxSocket::Shutdown(void)
{
  Abort();

  isyslog("Client %s disconnected", m_strAddress.c_str());

  CLockObject lock(m_abortMutex);

  delete m_pollerRead;
  m_pollerRead = NULL;

  delete m_pollerWrite;
  m_pollerWrite = NULL;

  shutdown(m_fd, SHUT_RD);

  close(m_fd);
}

bool CLinuxSocket::Read(std::string& buffer, unsigned int totalBytes)
{
  buffer.resize(totalBytes);

  int retryCounter = 0;

  ssize_t missing = static_cast<ssize_t>(totalBytes);

  uint8_t* ptr = reinterpret_cast<uint8_t*>(const_cast<char*>(buffer.data()));

  while (missing > 0)
  {
    m_pollerRead->Poll(-1);

    if (m_bStop)
      break;

    ssize_t p = read(m_fd, ptr, missing);

    if (p < 0)
    {
      if (retryCounter < 10 && (errno == EINTR || errno == EAGAIN))
      {
        dsyslog("CLinuxSocket: EINTR/EAGAIN during read(), retrying");
        retryCounter++;
        continue;
      }

      esyslog("CLinuxSocket: Read error at %d/%d", (int)(totalBytes - missing), (int)totalBytes);
      return false;
    }
    else if (p == 0)
    {
      dsyslog("CLinuxSocket: End of stream, connection closed");
      return false;
    }

    retryCounter = 0;
    ptr += p;
    missing -= p;
  }

  return true;
}

void CLinuxSocket::Abort(void)
{
  m_bStop = true;

  CLockObject lock(m_abortMutex);

  if (m_pollerRead)
    m_pollerRead->Abort();
}

bool CLinuxSocket::Write(const std::string& request)
{
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(request.data());

  size_t size = request.size();

  const ssize_t written = static_cast<ssize_t>(size);

  CLockObject lock(m_writeMutex);

  while (size > 0)
  {
    if (!m_pollerWrite->Poll(WRITE_POLL_TIMEOUT_MS))
    {
      esyslog("CLinuxSocket: Write timed out");
      return written - size;
    }

    const bool more_data = false;
    ssize_t p = send(m_fd, ptr, size, (more_data ? MSG_MORE : 0));

    if (p <= 0)
    {
      if (errno == EINTR || errno == EAGAIN)
      {
        dsyslog("CLinuxSocket: EINTR during write, retrying");
        continue;
      }
      else if (errno != EPIPE)
      {
        esyslog("CLinuxSocket: Write error");
      }
      return p;
    }

    ptr += p;
    size -= p;
  }

  return written;
}
