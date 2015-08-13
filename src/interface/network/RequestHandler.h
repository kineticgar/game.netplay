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

#include "RPCMethods.h"
#include "utils/Observer.h"

#include "platform/threads/mutex.h"
#include "platform/threads/threads.h"

#include <deque>
#include <stack>
#include <string>

namespace NETPLAY
{
  class CClient;

  class CRequestHandler : public PLATFORM::CThread,
                          public Observable
  {
  public:
    CRequestHandler(CClient* client);
    virtual ~CRequestHandler(void);

    virtual void ReceiveRequest(RPC_METHOD method, const std::string& strRequest);

  protected:
    CClient* const m_client;

    // implementation of CThread
    void* Process(void);

    virtual bool HandleRequest(RPC_METHOD method, const std::string& strRequest) = 0;

    virtual CRequestHandler* Clone(void) const = 0;

  private:
    struct Request
    {
      Request(RPC_METHOD method, const std::string& strRequest) :
        method(method),
        strRequest(strRequest)
      {
      }

      RPC_METHOD  method;
      std::string strRequest;
    };

    Request*         m_request;
    PLATFORM::CEvent m_requestEvent;
    PLATFORM::CMutex m_requestMutex;

    CRequestHandler* m_nextHandler; // To handle simultaneous requests
  };
}
