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

#include "interface/network/Connection.h"

#include "platform/threads/mutex.h"

#include <stddef.h>
#include <string>

namespace PLATFORM
{
  class CTcpConnection;
}

namespace NETPLAY
{
  class CClient : public CConnection
  {
  public:
    CClient(const std::string& strAddress, unsigned int port);
    virtual ~CClient(void);

    // implementation of IConnection
    virtual std::string Address(void) const;
    virtual bool Open(void);
    virtual void Close(void);

  protected:
    virtual bool SendData(const std::string& request);
    virtual bool ReadData(std::string& buffer, size_t totalBytes, unsigned int timeoutMs);

  private:
    const std::string         m_strAddress;
    const unsigned int        m_port;
    PLATFORM::CTcpConnection* m_socket;
    PLATFORM::CMutex          m_readMutex;
  };
}
