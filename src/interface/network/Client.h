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

#include "NetworkTypes.h"
#include "RPCMethods.h"

#include "utils/Observer.h"

#include "platform/threads/mutex.h"
#include "platform/threads/threads.h"

#include <string>
#include <vector>

namespace NETPLAY
{
  class IFrontend;
  class IGame;
  class CRequestHandler;

  class CClient : public Observer,
                  public Observable,
                  protected PLATFORM::CThread
  {
  public:
    CClient(const SocketPtr& socket, IGame* game);
    CClient(const SocketPtr& socket, IFrontend* frontend);

    virtual ~CClient(void);

    /*!
     * \brief Initialize this client instance
     */
    bool Initialize(void);

    /*!
     * \brief Deinitialize this client instance
     */
    void Deinitialize(void);

    void WaitForExit(void) { Sleep(0); }

    bool SendRequest(RPC_METHOD method, const std::string& strRequest);
    bool SendRequest(RPC_METHOD method, const std::string& strRequest, std::string& strResponse);
    bool SendResponse(RPC_METHOD method, const std::string& strResponse);

    // implementation of Observer
    void Notify(const Observable& obs, const ObservableMessage msg);

  protected:
    // implementation of CThead
    virtual void* Process(void);

  private:
    enum RPC_MESSAGE_TYPE
    {
      RPC_REQUEST,
      RPC_RESPONSE,
    };

    bool SendMessage(RPC_MESSAGE_TYPE messageType, RPC_METHOD method, const std::string& strMessage);

    struct Invocation
    {
      RPC_METHOD        method;
      std::string*      result;
      PLATFORM::CEvent* finished_event;
    };

    bool GetResponse(Invocation&  invocation,
                     RPC_METHOD   method,
                     std::string& response,
                     unsigned int iInitialTimeoutMs    = 10000,
                     unsigned int iDatapacketTimeoutMs = 10000);

    bool ReadHeader(RPC_MESSAGE_TYPE& messageType, RPC_METHOD& method, size_t& length);
    bool ReadResponse(RPC_METHOD method, size_t length);

    bool FormatHeader(RPC_MESSAGE_TYPE messageType, RPC_METHOD method, size_t length, std::string& header);
    bool ParseHeader(const std::string& header, RPC_MESSAGE_TYPE& messageType, RPC_METHOD& method, size_t& length);

    Invocation Invoke(RPC_METHOD method);
    void FreeInvocation(Invocation& invocation) const;

    const SocketPtr         m_socket;
    CRequestHandler* const  m_requestHandler;
    PLATFORM::CMutex        m_readMutex;
    PLATFORM::CMutex        m_writeMutex;
    std::vector<Invocation> m_invocations;
    PLATFORM::CMutex        m_invocationMutex;
  };
}
