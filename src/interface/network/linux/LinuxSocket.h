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

#include "interface/network/ISocket.h"

#include "platform/threads/mutex.h"

#include <string>

namespace NETPLAY
{
  class CLinuxPoller;

  class CLinuxSocket : public ISocket
  {
   public:
    CLinuxSocket(int fd);

    virtual ~CLinuxSocket(void) { Shutdown(); }

    // implementation of ISocket
    virtual bool Connect(void);
    virtual void Shutdown(void);
    virtual bool Read(std::string& buffer, unsigned int totalBytes);
    virtual bool Write(const std::string& request);

   private:
    void Abort(void);

    int              m_fd;
    std::string      m_strAddress;
    PLATFORM::CMutex m_abortMutex;
    PLATFORM::CMutex m_writeMutex;
    CLinuxPoller*    m_pollerRead;
    CLinuxPoller*    m_pollerWrite;
    volatile bool    m_bStop;
  };
}
