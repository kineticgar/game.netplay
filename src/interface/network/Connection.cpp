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
#include "log/Log.h"

using namespace NETPLAY;
using namespace PLATFORM;

#define MAX_MESSAGE_LENGTH     (5 * 1024 * 1024) // 5 MB
#define READ_TIMEOUT_MS        (5 * 1000) // 5 seconds

CConnection::CConnection(void) :
    m_bConnectionLost(false)
{
}

bool CConnection::Send(RPC_METHOD method, const std::string& request)
{
  if (!IsOpen())
    return false;

  if (!SendHeader(method, request.size()))
  {
    SignalConnectionLost();
    return false;
  }

  if (!SendData(request))
  {
    SignalConnectionLost();
    return false;
  }

  return true;
}

bool CConnection::Send(RPC_METHOD method, const std::string& request, std::string& response)
{
  if (!Send(method, request))
    return false;

  if (!GetResponse(method, response))
  {
    SignalConnectionLost();
    return false;
  }

  return true;
}

bool CConnection::SendHeader(RPC_METHOD method, size_t msgLength)
{
  std::string header;
  if (!FormatHeader(header, method, msgLength))
    return false;

  if (!SendData(header))
    return false;

  return true;
}

void* CConnection::Process(void)
{
  while (!IsStopped())
  {
    RPC_METHOD msgMethod;
    size_t msgLength;

    if (!ReadHeader(msgMethod, msgLength, 10000))
      break;

    if (!ReadMessage(msgMethod, msgLength, 10000))
      break;
  }

  return NULL;
}

bool CConnection::ReadMessage(RPC_METHOD method, size_t length, unsigned int timeoutMs)
{

  bool bFound = false;
  Invocation invocation;

  {
    CLockObject lock(m_invocationMutex);

    for (std::vector<Invocation>::iterator it = m_invocations.begin(); it != m_invocations.end(); ++it)
    {
      if (it->method == method)
      {
        bFound = true;
        invocation = *it;
        m_invocations.erase(it);
        break;
      }
    }
  }

  if (bFound)
  {
    invocation.result->reserve(length);

    if (!ReadData(*invocation.result, length, timeoutMs))
      return false;

    invocation.finished_event->Signal();
  }
  else
  {
    std::string dummy;
    ReadData(dummy, length, timeoutMs);
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

  if (invocation.finished_event->Wait(READ_TIMEOUT_MS))
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

bool CConnection::ReadHeader(RPC_METHOD& method, size_t& length, unsigned int timeoutMs)
{
  std::string header;
  header.resize(5);

  std::string message;
  if (!ReadData(message, 5, timeoutMs))
    return false;

  return ParseHeader(message, method, length);
}

void CConnection::SignalConnectionLost(void)
{
  if (m_bConnectionLost)
    return;

  esyslog("%s - connection lost !!!", __FUNCTION__);

  m_bConnectionLost = true;
  Close();

  OnDisconnect();
}

bool CConnection::FormatHeader(std::string& header, RPC_METHOD method, size_t length)
{
  header.resize(5);

  header[0] = static_cast<uint16_t>(method) >> 8;
  header[1] = static_cast<uint16_t>(method) & 0xff;
  header[2] = length >> 16;
  header[3] = (length >> 8) & 0xff;
  header[4] = length & 0xff;

  return true;
}

bool CConnection::ParseHeader(const std::string& header, RPC_METHOD& method, size_t& length)
{
  if (header.size() != 5)
    return false;

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


/* TODO
try
{
  // TODO
  uint32_t    version       = vresp->extract_U32();
  uint32_t    vdrTime       = vresp->extract_U32();
  int32_t     vdrTimeOffset = vresp->extract_S32();
  std::string ServerName    = vresp->extract_String();
  std::string ServerVersion = vresp->extract_String();

  std::string strVersion;
  uint32_t    vdrTime;
  int32_t     vdrTimeOffset;
  std::string ServerName;
  std::string ServerVersion;

  m_server    = ServerName;
  m_version   = ServerVersion;
  m_protocol  = protocol;

  if (m_protocol < VNSI_MIN_PROTOCOLVERSION)
    throw "Protocol versions do not match";

  isyslog("Logged in at '%lu + %i' to '%s' Version: '%s' with protocol version '%d'", vdrTime, vdrTimeOffset, ServerName, ServerVersion, protocol);
  throw "Not implemented";
}
catch (const char* strError)
{
  esyslog("%s - %s", __FUNCTION__, strError);
  if (m_socket)
  {
    m_socket->Close();
    delete m_socket;
    m_socket = NULL;
  }
  return false;
}
*/
