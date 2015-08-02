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
#pragma once

#include "interface/network/ISocket.h"

#include "platform/threads/mutex.h"
#include "platform/sockets/tcp.h"

namespace NETPLAY
{
  class CPlatformSocket : public ISocket
  {
  public:
    CPlatformSocket(const std::string& strAddress, unsigned int port);

    virtual ~CPlatformSocket(void) { Shutdown(); }

    // implementation of ISocket
    virtual bool Connect(void);
    virtual void Shutdown(void);
    virtual bool Read(std::string& buffer, unsigned int totalBytes);
    virtual bool Abort(void);
    virtual bool Write(const std::string& request);

  private:
    std::string Address(void) const;

    const std::string         m_strAddress;
    const unsigned int        m_port;
    PLATFORM::CTcpConnection* m_socket;
    PLATFORM::CMutex          m_readMutex;
  };
}
