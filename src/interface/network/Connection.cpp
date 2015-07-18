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

#define MAX_MESSAGE_LENGTH     (16 * 1024 * 1024) // 16 MB

CConnection::CConnection(void) :
    m_bConnectionLost(false)
{
}

bool CConnection::Send(RPC_METHOD method, const std::string& request)
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

bool CConnection::Send(RPC_METHOD method, const std::string& request, std::string& response)
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

void* CConnection::Process(void)
{
  while (!IsStopped())
  {
    RPC_METHOD msgMethod = RPC_METHOD::Invalid;
    size_t msgLength = 0;

    CLockObject lock(m_readMutex);

    if (!ReadHeader(msgMethod, msgLength, 10000))
      break;

    // Validate input
    if (msgLength > MAX_MESSAGE_LENGTH)
    {
      esyslog("Terminating connection - invalid message length: %d", msgLength);
      break;
    }

    std::string message;
    message.resize(msgLength);
    if (!ReadData(message, msgLength, 10000))
      break;

    ReceiveMessage(msgMethod, message);

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
  }

  return NULL;
}

void CConnection::ReceiveMessage(RPC_METHOD method, const std::string& message)
{
  // TODO
}

bool CConnection::ReadHeader(RPC_METHOD& method, size_t& length, unsigned int timeoutMs)
{
  std::string header;
  if (ReadData(header, 5, timeoutMs))
  {
    method = static_cast<RPC_METHOD>(header[0] << 8  | header[1]);
    length = header[2] << 16 | header[3] << 8 | header[4];
    return true;
  }

  return false;
}

bool CConnection::ReadMessage(RPC_METHOD   method,
                              std::string& response,
                              unsigned int iInitialTimeoutMs    /* = 10000 */,
                              unsigned int iDatapacketTimeoutMs /* = 10000 */)
{
  RPC_METHOD msgMethod = RPC_METHOD::Invalid;
  size_t msgLength = 0;

  CLockObject lock(m_readMutex);

  while (msgMethod != method) // TODO
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

bool CConnection::SendHeader(RPC_METHOD method, size_t msgLength)
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

void CConnection::SignalConnectionLost(void)
{
  if (m_bConnectionLost)
    return;

  esyslog("%s - connection lost !!!", __FUNCTION__);

  m_bConnectionLost = true;
  Close();

  OnDisconnect();
}
