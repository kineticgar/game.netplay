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
#include "log/Log.h"
#include "utils/StringUtils.h"

#include "platform/sockets/tcp.h"
#include "platform/threads/mutex.h"

using namespace NETPLAY;
using namespace PLATFORM;

#define CONNECTION_TIMEOUT_MS  2000
#define MAX_MESSAGE_LENGTH     (16 * 1024 * 1024) // 16 MB

CClient::CClient(const std::string& strAddress, unsigned int port) :
  m_strAddress(strAddress),
  m_port(port),
  m_socket(NULL),
  m_readMutex(new CMutex),
  m_bConnectionLost(true)
{
}

CClient::~CClient(void)
{
  Close();
  delete m_readMutex;
}

bool CClient::Open(void)
{
  Close();

  uint64_t iNow = GetTimeMs();
  uint64_t iTarget = iNow + CONNECTION_TIMEOUT_MS;

  if (!m_socket)
    m_socket = new CTcpConnection(m_strAddress.c_str(), m_port);

  while (!m_socket->IsOpen() && iNow < iTarget)
  {
    if (!m_socket->Open(iTarget - iNow))
      CEvent::Sleep(100);

    iNow = GetTimeMs();
  }

  if (!m_socket->IsOpen())
  {
    esyslog("%s - failed to connect to the server (%s)", __FUNCTION__, m_socket->GetError().c_str());
    return false;
  }

  return true;
}

bool CClient::IsOpen(void)
{
  bool bReturn(false);

  if (m_socket && m_socket->IsOpen())
    bReturn = true;

  return bReturn;
}

void CClient::Close(void)
{
  if (IsOpen())
    m_socket->Close();

  if (m_socket)
  {
    delete m_socket;
    m_socket = NULL;
  }
}

bool CClient::Send(RPC_METHOD method, const std::string& request)
{
  if (!IsOpen())
    return false;

  try
  {
    if (!SendHeader(method, request.length()))
      throw false;

    if (!SendData(request))
      throw false;
  }
  catch (bool bSuccess)
  {
    SignalConnectionLost();
    return bSuccess;
  }

  return true;
}

bool CClient::Send(RPC_METHOD method, const std::string& request, std::string& response)
{
  if (!Send(method, request))
    return false;

  try
  {
    if (!ReadMessage(method, response))
      throw false;
  }
  catch (bool bSuccess)
  {
    SignalConnectionLost();
    return bSuccess;
  }

  return true;
}

bool CClient::SendHeader(RPC_METHOD method, size_t msgLength)
{
  std::string header;
  header.resize(5);
  header[0] = static_cast<uint16_t>(method) >> 8;
  header[1] = static_cast<uint16_t>(method) & 0xff;
  header[2] = msgLength >> 16;
  header[3] = (msgLength >> 8) & 0xff;
  header[4] = msgLength & 0xff;
  return SendData(header);
}

bool CClient::SendData(const std::string& request)
{
  ssize_t iWriteResult = m_socket->Write(const_cast<char*>(request.c_str()), request.length());
  if (iWriteResult != static_cast<ssize_t>(request.length()))
  {
    esyslog("%s - Failed to write packet (%s), bytes written: %d of total: %d", __FUNCTION__, m_socket->GetError().c_str(), iWriteResult, request.length());
    return false;
  }
  return true;
}

bool CClient::ReadMessage(RPC_METHOD method,
                          std::string& response,
                          unsigned int iInitialTimeoutMs /* = 10000 */,
                          unsigned int iDatapacketTimeoutMs /* = 10000 */)
{
  RPC_METHOD msgMethod = RPC_METHOD::Invalid;
  size_t msgLength = 0;

  CLockObject lock(*m_readMutex);

  while (msgMethod != method)
  {
    if (!ReadHeader(msgMethod, msgLength, iInitialTimeoutMs))
      return false;
  }

  // Validate input
  if (msgLength > MAX_MESSAGE_LENGTH)
    return false;

  response.resize(msgLength);
  return ReadData(response, msgLength, iDatapacketTimeoutMs);
}

bool CClient::ReadHeader(RPC_METHOD& method, size_t& length, unsigned int timeoutMs)
{
  std::string header;
  if (ReadData(header, 2, timeoutMs))
  {
    method = static_cast<RPC_METHOD>(header[0] << 8  | header[1]);
    length = header[2] << 16 | header[3] << 8 | header[4];
    return true;
  }

  return false;
}

bool CClient::ReadData(std::string& buffer, size_t totalBytes, unsigned int timeoutMs)
{
  unsigned int bytesRead = 0;

  buffer.resize(totalBytes);
  bytesRead += m_socket->Read(const_cast<char*>(buffer.c_str()), totalBytes, timeoutMs);

  if (bytesRead != totalBytes)
  {
    if (m_socket->GetErrorNumber() == ETIMEDOUT && bytesRead > 0)
    {
      // We read something, so try to finish the read
      bytesRead += m_socket->Read(const_cast<char*>(buffer.c_str()) + bytesRead,
                                  totalBytes - bytesRead,
                                  timeoutMs);
    }
    else if (m_socket->GetErrorNumber() == ETIMEDOUT)
    {
      SignalConnectionLost();
    }
  }

  return bytesRead == totalBytes;
}

void CClient::SignalConnectionLost(void)
{
  if (m_bConnectionLost)
    return;

  esyslog("%s - connection lost !!!", __FUNCTION__);

  m_bConnectionLost = true;
  Close();

  OnDisconnect();
}

std::string CClient::Address(void) const
{
  return StringUtils::Format("%s:d", m_strAddress.c_str(), m_port);
}
