/*
 *      Copyright (C) 2013-2015 Garrett Brown
 *      Copyright (C) 2015 Team XBMC
 *      Portions Copyright (C) 2013-2014 Lars Op den Kamp
 *      Portions Copyright (C) 2000, 2003, 2006, 2008, 2013 Klaus Schmidinger
 *      Portions Copyright (C) 2005-2013 Team XBMC
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

#include "LinuxPoller.h"
#include "log/Log.h"

#include <fcntl.h>
#include <sys/poll.h>
#include <unistd.h>

using namespace NETPLAY;
using namespace PLATFORM;

CLinuxPoller::CLinuxPoller(int  fileHandle    /* = -1    */,
                           bool bOut          /* = false */,
                           bool bPriorityOnly /* = false */) :
  m_numFileHandles(0)
{
  m_selfPipe[0] = -1;
  m_selfPipe[1] = -1;

  if (pipe(m_selfPipe) != -1)
  {
    fcntl(m_selfPipe[0], F_SETFL, fcntl(m_selfPipe[0], F_GETFL) | O_NONBLOCK);
    fcntl(m_selfPipe[1], F_SETFL, fcntl(m_selfPipe[1], F_GETFL) | O_NONBLOCK);

    Add(m_selfPipe[0], false);
  }
  else
  {
    esyslog("CLinuxPoller: Failed to create self-pipe");
  }

  Add(fileHandle, bOut, bPriorityOnly);
}

bool CLinuxPoller::Add(int fileHandle, bool bOut, bool bPriorityOnly /* = false */)
{
  if (fileHandle >= 0)
  {
    const short int requestedEvents = (bOut ? POLLOUT : POLLIN) |
                                      (bPriorityOnly ? POLLPRI : 0);

    // Look for duplicates
    for (unsigned int i = 0; i < m_numFileHandles; i++)
    {
      if (m_pfd[i].fd == fileHandle && m_pfd[i].events == requestedEvents)
        return true;
    }

    if (m_numFileHandles < MaxPollFiles)
    {
      m_pfd[m_numFileHandles].fd = fileHandle;
      m_pfd[m_numFileHandles].events = requestedEvents;
      m_pfd[m_numFileHandles].revents = 0;
      m_numFileHandles++;
      return true;
    }

    esyslog("ERROR: too many file handles in CLinuxPoller");
  }
  return false;
}

bool CLinuxPoller::Poll(int timeoutMs /* = 0 */)
{
  if (m_numFileHandles)
  {
    if (poll(m_pfd, m_numFileHandles, timeoutMs) != 0)
    {
      // Flush pipe
      if (m_selfPipe[0] != -1)
      {
        char dummy;
        while (read(m_selfPipe[0], &dummy, 1) == 1)
          ;
      }
      // returns true even in case of an error, to let the caller
      // access the file and thus see the error code
      return true;
    }
  }
  return false;
}

void CLinuxPoller::Abort(void)
{
  if (m_selfPipe[1] != -1)
    write(m_selfPipe[1], "", 1);
}
