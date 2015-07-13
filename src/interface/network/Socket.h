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
#pragma once

#include "platform/threads/mutex.h"

#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

namespace NETPLAY
{
  class CPoller;

  class CSocket
  {
   public:
    CSocket(void);
    ~CSocket(void);

    void SetHandle(int h);

    void Close(void);

    void Shutdown(void);

    void LockWrite(void);

    void UnlockWrite(void);

    ssize_t Read(uint8_t* buffer, size_t size, int timeout_ms = -1);

    ssize_t Write(const uint8_t* buffer, size_t size, int timeout_ms = -1, bool more_data = false);

    static char* ip2txt(uint32_t ip, unsigned int port, char* str);

   private:
    int              m_fd;
    PLATFORM::CMutex m_MutexWrite;
    CPoller*         m_pollerRead;
    CPoller*         m_pollerWrite;
  };
}
