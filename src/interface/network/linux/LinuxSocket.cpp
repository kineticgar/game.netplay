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
#include <net/if.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>

using namespace PLATFORM;
using namespace NETPLAY;

#ifndef MSG_MORE
  #define MSG_MORE 0
#endif

CLinuxSocket::CLinuxSocket(void) :
  m_fd(-1),
  m_pollerRead(NULL),
  m_pollerWrite(NULL)
{
}

CLinuxSocket::~CLinuxSocket(void)
{
  Close();

  delete m_pollerRead;
  delete m_pollerWrite;
}

void CLinuxSocket::Close(void)
{
  CLockObject lock(m_MutexWrite);

  if (m_fd >= 0)
  {
    close(m_fd);

    m_fd = -1;

    delete m_pollerRead;
    m_pollerRead = NULL;

    delete m_pollerWrite;
    m_pollerWrite = NULL;
  }
}

void CLinuxSocket::Shutdown(void)
{
  CLockObject lock(m_MutexWrite);

  if (m_fd >= 0)
  {
    shutdown(m_fd, SHUT_RD);
  }
}

ssize_t CLinuxSocket::Write(const uint8_t* buffer, size_t size, int timeout_ms /* = -1 */,
                                                           bool more_data /* = false */)
{
  CLockObject lock(m_MutexWrite);

  if (m_fd == -1)
    return -1;

  ssize_t written = static_cast<ssize_t>(size);

  const uint8_t* ptr = buffer;

  while (size > 0)
  {
    if (!m_pollerWrite->Poll(timeout_ms))
    {
      esyslog("CLinuxSocket::write: poll() failed");
      return written-size;
    }

    ssize_t p = send(m_fd, ptr, size, (more_data ? MSG_MORE : 0));

    if (p <= 0)
    {
      if (errno == EINTR || errno == EAGAIN)
      {
        dsyslog("CLinuxSocket::write: EINTR during write(), retrying");
        continue;
      }
      else if (errno != EPIPE)
      {
        esyslog("CLinuxSocket::write: write() error");
      }
      return p;
    }

    ptr += p;
    size -= p;
  }

  return written;
}

ssize_t CLinuxSocket::Read(uint8_t* buffer, size_t size, int timeout_ms)
{
  int retryCounter = 0;

  if (m_fd == -1)
    return -1;

  ssize_t missing = static_cast<ssize_t>(size);

  uint8_t* ptr = buffer;

  while (missing > 0)
  {
    if (m_pollerRead->Poll(timeout_ms) == 0)
    {
      //esyslog("CLinuxSocket::read: poll() failed at %d/%d", (int)(size - missing), (int)size);
      return size - missing;
    }

    ssize_t p = read(m_fd, ptr, missing);

    if (p < 0)
    {
      if (retryCounter < 10 && (errno == EINTR || errno == EAGAIN))
      {
        dsyslog("CLinuxSocket::Read - EINTR/EAGAIN during read(), retrying");
        retryCounter++;
        continue;
      }

      esyslog("CLinuxSocket::Read - read() error at %d/%d", (int)(size - missing), (int)size);
      return 0;
    }
    else if (p == 0)
    {
      dsyslog("CLinuxSocket::Read - end of stream, connection closed");
      Close();
      return 0;
    }

    retryCounter = 0;
    ptr  += p;
    missing -= p;
  }

  return size;
}

void CLinuxSocket::SetHandle(int h)
{
  CLockObject lock(m_MutexWrite);

  if (h != m_fd)
  {
    Close();

    m_fd          = h;
    m_pollerRead  = new CLinuxPoller(m_fd);
    m_pollerWrite = new CLinuxPoller(m_fd, true);
  }
}
