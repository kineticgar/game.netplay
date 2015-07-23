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

#include "IConnection.h"

#include "platform/threads/mutex.h"
#include "platform/threads/threads.h"

#include <string>
#include <vector>

namespace NETPLAY
{
  class IFrontend;
  class IGame;
  class IRequestHandler;

  class CConnection : public IConnection,
                      protected PLATFORM::CThread
  {
  public:
    CConnection(IFrontend* frontend);
    CConnection(IGame* game);
    virtual ~CConnection(void);

    // partial implementation of IConnection
    virtual bool Open(void) { return CreateThread(); }
    virtual void Close(void) { StopThread(); }
    virtual bool SendRequest(RPC_METHOD method, const std::string& strRequest);
    virtual bool SendRequest(RPC_METHOD method, const std::string& strRequest, std::string& strResponse);
    virtual bool SendResponse(RPC_METHOD method, const std::string& strResponse);

  protected:
    // implementation of CThread
    virtual void* Process(void);

    virtual bool ReadData(std::string& buffer, size_t totalBytes, unsigned int timeoutMs) = 0;
    virtual bool SendData(const std::string& request) = 0;

    void SignalConnectionLost(void);
    bool IsConnectionLost(void) const { return m_bConnectionLost; }

  private:
    struct Invocation
    {
      RPC_METHOD        method;
      std::string*      result;
      PLATFORM::CEvent* finished_event;
    };

    bool GetResponse(RPC_METHOD   method,
                     std::string& response,
                     unsigned int iInitialTimeoutMs    = 10000,
                     unsigned int iDatapacketTimeoutMs = 10000);

    bool SendHeader(bool bRequest, RPC_METHOD method, size_t msgLength);

    bool ReadHeader(bool& bRequest, RPC_METHOD& method, size_t& length);
    bool ReadRequest(RPC_METHOD method, size_t length);
    bool ReadResponse(RPC_METHOD method, size_t length);

    bool FormatHeader(std::string& header, bool bRequest, RPC_METHOD method, size_t length);
    bool ParseHeader(const std::string& header, bool& bRequest, RPC_METHOD& method, size_t& length);

    Invocation Invoke(RPC_METHOD method);
    void FreeInvocation(Invocation& invocation) const;

    IRequestHandler* const  m_requestHandler;
    std::vector<Invocation> m_invocations;
    PLATFORM::CMutex        m_invocationMutex;
    bool                    m_bConnectionLost;
    // TODO: write mutex
  };
}
