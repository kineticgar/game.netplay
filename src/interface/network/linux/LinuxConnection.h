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

#include "LinuxSocket.h"
#include "interface/network/IConnection.h"
#include "interface/network/RPCMethods.h"

#include "platform/threads/mutex.h"
#include "platform/threads/threads.h"

#include <string>

namespace NETPLAY
{
  class CLinuxConnection : public IConnection,
                           protected PLATFORM::CThread
  {
  public:
    CLinuxConnection(int fd);
    virtual ~CLinuxConnection(void) { Close(); }

    // implementation of IConnection
    virtual bool Open(void);
    virtual void Close(void);
    virtual std::string Address(void) const { return m_strClientAddress; }
    virtual bool IsOpen(void) { return IsRunning(); }
    virtual bool Send(RPC_METHOD method, const std::string& request) { return false; } // TODO
    virtual bool Send(RPC_METHOD method, const std::string& request, std::string& response) { return false; } // TODO

  private:
    bool SendHeader(RPC_METHOD method, size_t msgLength);
    bool SendData(const std::string& request);

    bool ReadMessage(RPC_METHOD method,
                     std::string& response,
                     unsigned int iInitialTimeoutMs = 10000,
                     unsigned int iDatapacketTimeoutMs = 10000);
    bool ReadHeader(RPC_METHOD& method, size_t& length, unsigned int timeoutMs);
    bool ReadData(std::string& buffer, size_t totalBytes, unsigned int timeoutMs);

    void OnDisconnect(void) { }
    void OnReconnect(void) { }

    void SignalConnectionLost(void);
    bool IsConnectionLost(void) const { return m_bConnectionLost; }

  protected:
    // implementation of PLATFORM::CThread
    virtual void* Process(void);

  private:
    int               m_fd;
    std::string       m_strClientAddress;
    CLinuxSocket      m_socket;
    PLATFORM::CMutex  m_readMutex;
    bool              m_bConnectionLost;
  };
}
