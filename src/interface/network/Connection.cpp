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

#include "Connection.h"
#include "FrontendHandler.h"
#include "GameHandler.h"
#include "log/Log.h"

#include <assert.h>

using namespace NETPLAY;
using namespace PLATFORM;

#define MAX_MESSAGE_LENGTH     (5 * 1024 * 1024) // 5 MB
#define RESPONSE_TIMEOUT_MS    (5 * 1000) // 5 seconds
#define READ_TIMEOUT_MS        (10 * 1000) // 10 seconds
#define REQUEST_MASK           0x80 // MSB

CConnection::CConnection(IFrontend* frontend) :
  m_requestHandler(new CFrontendHandler(frontend)),
  m_bConnectionLost(false)
{
}

CConnection::CConnection(IGame* game) :
  m_requestHandler(new CGameHandler(game)),
  m_bConnectionLost(false)
{
}

CConnection::~CConnection(void)
{
  delete m_requestHandler;
}

bool CConnection::SendRequest(RPC_METHOD method, const std::string& strRequest)
{
  if (!SendHeader(true, method, strRequest.size()))
  {
    SignalConnectionLost();
    return false;
  }

  if (!SendData(strRequest))
  {
    SignalConnectionLost();
    return false;
  }

  return true;
}

bool CConnection::SendRequest(RPC_METHOD method, const std::string& strRequest, std::string& strResponse)
{
  if (!SendRequest(method, strRequest))
    return false;

  if (!GetResponse(method, strResponse))
  {
    SignalConnectionLost();
    return false;
  }

  return true;
}

bool CConnection::SendResponse(RPC_METHOD method, const std::string& strResponse)
{
  if (!SendHeader(false, method, strResponse.size()))
  {
    SignalConnectionLost();
    return false;
  }

  if (!SendData(strResponse))
  {
    SignalConnectionLost();
    return false;
  }

  return true;
}

bool CConnection::SendHeader(bool bRequest, RPC_METHOD method, size_t msgLength)
{
  std::string header;
  if (!FormatHeader(header, bRequest, method, msgLength))
    return false;

  if (!SendData(header))
    return false;

  return true;
}

void* CConnection::Process(void)
{
  while (!IsStopped())
  {
    bool bRequest;
    RPC_METHOD msgMethod;
    size_t msgLength;

    if (!ReadHeader(bRequest, msgMethod, msgLength))
      break;

    if (bRequest)
    {
      if (!ReadRequest(msgMethod, msgLength))
        break;
    }
    else
    {
      if (!ReadResponse(msgMethod, msgLength))
        break;
    }
  }

  return NULL;
}

bool CConnection::ReadRequest(RPC_METHOD method, size_t length)
{
  std::string strRequest;
  if (!ReadData(strRequest, length, READ_TIMEOUT_MS))
    return false;

  return m_requestHandler->HandleRequest(method, strRequest, this);
}

bool CConnection::ReadResponse(RPC_METHOD method, size_t length)
{

  bool bInvocationFound = false;
  Invocation invocation;

  {
    CLockObject lock(m_invocationMutex);

    for (std::vector<Invocation>::iterator it = m_invocations.begin(); it != m_invocations.end(); ++it)
    {
      if (it->method == method)
      {
        bInvocationFound = true;
        invocation = *it;
        m_invocations.erase(it);
        break;
      }
    }
  }

  if (bInvocationFound)
  {
    invocation.result->reserve(length);

    if (!ReadData(*invocation.result, length, READ_TIMEOUT_MS))
      return false;

    invocation.finished_event->Signal();
  }
  else
  {
    std::string dummy;
    if (!ReadData(dummy, length, READ_TIMEOUT_MS))
      return false;
  }

  return true;
}

bool CConnection::GetResponse(RPC_METHOD   method,
                              std::string& response,
                              unsigned int iInitialTimeoutMs    /* = 10000 */,
                              unsigned int iDatapacketTimeoutMs /* = 10000 */)
{
  bool bSuccess = false;

  Invocation invocation = Invoke(method);

  {
    CLockObject lock(m_invocationMutex);
    m_invocations.push_back(invocation);
  }

  if (invocation.finished_event->Wait(RESPONSE_TIMEOUT_MS))
  {
    // Event was signaled and invocation was removed from array
    response = *invocation.result;
    bSuccess = true;
  }
  else
  {
    // Event wasn't signaled, need to remove invocation from the array
    dsyslog("Timed out waiting for event (method=%d)", method);

    CLockObject lock(m_invocationMutex);

    for (std::vector<Invocation>::iterator it = m_invocations.begin(); it != m_invocations.end(); ++it)
    {
      if (it->method == method)
      {
        m_invocations.erase(it);
        break;
      }
    }
  }

  FreeInvocation(invocation);

  return bSuccess;
}

bool CConnection::ReadHeader(bool& bRequest, RPC_METHOD& method, size_t& length)
{
  std::string header;
  header.resize(5);

  std::string message;
  if (!ReadData(message, 5, READ_TIMEOUT_MS))
    return false;

  return ParseHeader(message, bRequest, method, length);
}

void CConnection::SignalConnectionLost(void)
{
  if (m_bConnectionLost)
    return;

  esyslog("%s - connection lost !!!", __FUNCTION__);

  m_bConnectionLost = true;

  SetChanged();
  NotifyObservers(ObservableMessageConnectionLost);

  Close();
}

bool CConnection::FormatHeader(std::string& header, bool bRequest, RPC_METHOD method, size_t length)
{
  header.resize(5);

  header[0] = (static_cast<uint16_t>(method) >> 8) | (bRequest ? REQUEST_MASK : 0);
  header[1] = static_cast<uint16_t>(method) & 0xff;
  header[2] = length >> 16;
  header[3] = (length >> 8) & 0xff;
  header[4] = length & 0xff;

  return true;
}

bool CConnection::ParseHeader(const std::string& header, bool& bRequest, RPC_METHOD& method, size_t& length)
{
  if (header.size() != 5)
    return false;

  bRequest = (header[0] & REQUEST_MASK) ? true : false;

  method = static_cast<RPC_METHOD>(header[0] << 8  | header[1]);
  if (static_cast<int>(method) >= static_cast<int>(RPC_METHOD::RPC_METHOD_COUNT))
  {
    esyslog("Terminating connection - invalid method: %d", method);
    return false;
  }

  length = header[2] << 16 | header[3] << 8 | header[4];
  if (length > MAX_MESSAGE_LENGTH)
  {
    esyslog("Terminating connection - invalid message length: %d", length);
    return false;
  }

  return true;
}

CConnection::Invocation CConnection::Invoke(RPC_METHOD method)
{
  Invocation invocation;

  invocation.method         = method;
  invocation.finished_event = new CEvent;
  invocation.result         = new std::string;

  return invocation;
}

void CConnection::FreeInvocation(Invocation& invocation) const
{
  delete invocation.finished_event;
  invocation.finished_event = NULL;

  delete invocation.result;
  invocation.result = NULL;
}
