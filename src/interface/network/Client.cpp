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

#include "Client.h"
#include "FrontendHandler.h"
#include "GameHandler.h"
#include "ISocket.h"
#include "log/Log.h"

#include <assert.h>

using namespace NETPLAY;
using namespace PLATFORM;

#define HEADER_SIZE            5 // bytes
#define MAX_MESSAGE_LENGTH     (5 * 1024 * 1024) // 5 MB
#define RESPONSE_TIMEOUT_MS    (5 * 1000) // 5 seconds
#define REQUEST_MASK           0x80 // MSB

CClient::CClient(const SocketPtr& socket, IGame* game) :
  m_socket(socket),
  m_requestHandler(new CGameHandler(this, game))
{
  assert(m_socket.get() != NULL);
  m_requestHandler->RegisterObserver(this);
}

CClient::CClient(const SocketPtr& socket, IFrontend* frontend) :
  m_socket(socket),
  m_requestHandler(new CFrontendHandler(this, frontend))
{
  assert(m_socket.get() != NULL);
  m_requestHandler->RegisterObserver(this);
}

CClient::~CClient(void)
{
  Deinitialize();
  m_requestHandler->UnregisterObserver(this);
  delete m_requestHandler;
}

bool CClient::Initialize(void)
{
  isyslog("Netplay client starting up");

  if (!m_socket->Connect())
    return false;

  return CreateThread();
}

void CClient::Deinitialize(void)
{
  if (IsRunning())
  {
    isyslog("Netplay client shutting down");
    StopThread(-1);
  }

  m_socket->Shutdown();

  StopThread(0);
}

void* CClient::Process(void)
{
  dsyslog("Starting client thread");

  while (!IsStopped())
  {
    RPC_MESSAGE_TYPE messageType;
    size_t msgLength;
    RPC_METHOD msgMethod;
    std::string strRequest;

    {
      CLockObject lock(m_readMutex);

      if (!ReadHeader(messageType, msgMethod, msgLength))
        break;

      dsyslog("Received %s: method=%s, length=%u", messageType == RPC_REQUEST ? "request" : "response",
          RPCMethods::TranslateMethod(msgMethod), msgLength);

      if (messageType == RPC_REQUEST)
      {
        if (msgLength > 0)
        {
          if (!m_socket->Read(strRequest, msgLength))
            break;
        }
      }
      else
      {
        if (!ReadResponse(msgMethod, msgLength))
          break;
      }
    }

    if (messageType == RPC_REQUEST)
      m_requestHandler->ReceiveRequest(msgMethod, strRequest);
  }

  dsyslog("Ending client thread");

  SetChanged();
  NotifyObservers(ObservableMessageConnectionLost);

  return NULL;
}

bool CClient::SendRequest(RPC_METHOD method, const std::string& strRequest)
{
  Invocation invocation = Invoke(method);

  {
    CLockObject lock(m_invocationMutex);
    m_invocations.push_back(invocation);
  }

  return SendMessage(RPC_REQUEST, method, strRequest);
}

bool CClient::SendRequest(RPC_METHOD method, const std::string& strRequest, std::string& strResponse)
{
  Invocation invocation = Invoke(method);

  {
    CLockObject lock(m_invocationMutex);
    m_invocations.push_back(invocation);
  }

  if (!SendMessage(RPC_REQUEST, method, strRequest))
    return false;

  if (!GetResponse(invocation, method, strResponse))
  {
    esyslog("Server failed to respond, method=%s", RPCMethods::TranslateMethod(method));
    return false;
  }

  return true;
}

bool CClient::SendResponse(RPC_METHOD method, const std::string& strResponse)
{
  return SendMessage(RPC_RESPONSE, method, strResponse);
}

bool CClient::SendMessage(RPC_MESSAGE_TYPE messageType, RPC_METHOD method, const std::string& strMessage)
{
  if (IsStopped())
    return false;

  std::string header;
  if (!FormatHeader(messageType, method, strMessage.length(), header))
    return false;

  CLockObject lock(m_writeMutex);

  if (!m_socket->Write(header))
    return false;

  if (!strMessage.empty())
  {
    if (!m_socket->Write(strMessage))
      return false;
  }

  dsyslog("Sent %s: method=%s, length=%u", messageType == RPC_REQUEST ? "request" : "response",
      RPCMethods::TranslateMethod(method), strMessage.length());

  return true;
}

bool CClient::ReadResponse(RPC_METHOD method, size_t length)
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

    if (!m_socket->Read(*invocation.result, length))
      return false;

    invocation.finished_event->Signal();
  }
  else
  {
    esyslog("Received response, but method wasn't invoked!");
    std::string dummy;
    if (!m_socket->Read(dummy, length))
      return false;
  }

  return true;
}

bool CClient::GetResponse(Invocation&  invocation,
                          RPC_METHOD   method,
                          std::string& response,
                          unsigned int iInitialTimeoutMs    /* = 10000 */,
                          unsigned int iDatapacketTimeoutMs /* = 10000 */)
{
  bool bSuccess = false;

  if (invocation.finished_event->Wait(RESPONSE_TIMEOUT_MS))
  {
    // Event was signaled and invocation was removed from array
    response = *invocation.result;
    bSuccess = true;
  }
  else
  {
    // Event wasn't signaled, need to remove invocation from the array
    dsyslog("Timed out waiting for event (method=%s)", RPCMethods::TranslateMethod(method));

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

bool CClient::ReadHeader(RPC_MESSAGE_TYPE& messageType, RPC_METHOD& method, size_t& length)
{
  std::string header;
  header.resize(HEADER_SIZE);

  std::string message;
  if (!m_socket->Read(message, HEADER_SIZE))
    return false;

  return ParseHeader(message, messageType, method, length);
}

bool CClient::FormatHeader(RPC_MESSAGE_TYPE messageType, RPC_METHOD method, size_t length, std::string& header)
{
  header.resize(HEADER_SIZE);

  header[0] = (static_cast<uint16_t>(method) >> 8) | (messageType == RPC_REQUEST ? REQUEST_MASK : 0);
  header[1] = static_cast<uint16_t>(method) & 0xff;
  header[2] = length >> 16;
  header[3] = (length >> 8) & 0xff;
  header[4] = length & 0xff;

  return true;
}

bool CClient::ParseHeader(const std::string& header, RPC_MESSAGE_TYPE& messageType, RPC_METHOD& method, size_t& length)
{
  if (header.size() != 5)
    return false;

  const uint8_t* data = reinterpret_cast<const uint8_t*>(header.c_str());

  messageType = (data[0] & REQUEST_MASK) ? RPC_REQUEST : RPC_RESPONSE;

  method = static_cast<RPC_METHOD>(((data[0] & ~REQUEST_MASK) << 8)  | data[1]);
  if (static_cast<int>(method) >= static_cast<int>(RPC_METHOD::RPC_METHOD_COUNT))
  {
    esyslog("Terminating connection - invalid method: %d", method);
    return false;
  }

  length = data[2] << 16 | data[3] << 8 | data[4];
  if (length > MAX_MESSAGE_LENGTH)
  {
    esyslog("Terminating connection - invalid message length: %d", length);
    return false;
  }

  return true;
}

CClient::Invocation CClient::Invoke(RPC_METHOD method)
{
  Invocation invocation;

  invocation.method         = method;
  invocation.finished_event = new CEvent;
  invocation.result         = new std::string;

  return invocation;
}

void CClient::FreeInvocation(Invocation& invocation) const
{
  delete invocation.finished_event;
  invocation.finished_event = NULL;

  delete invocation.result;
  invocation.result = NULL;
}

void CClient::Notify(const Observable& obs, const ObservableMessage msg)
{
  switch (msg)
  {
    case ObservableMessageLoggedIn:
    {
      SetChanged();
      NotifyObservers(msg);
      break;
    }
    default:
      break;
  }
}
