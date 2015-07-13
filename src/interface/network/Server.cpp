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

#include "Server.h"

#include "platform/sockets/tcp.h"
#include "platform/threads/mutex.h"

using namespace NETPLAY;
using namespace PLATFORM;

#define CONNECTION_TIMEOUT_MS  2000
#define MAX_MESSAGE_LENGTH     (16 * 1024 * 1024) // 16 MB

// --- Version -----------------------------------------------------------------

namespace NETPLAY
{
  struct Version
  {
  public:
    Version(const std::string& strVersion)
    {
      major = 0; // TODO
      minor = 0; // TODO
      point = 0; // TODO
    }

    bool operator<(const Version& rhs) const
    {
      if (major < rhs.major) return true;
      if (major > rhs.major) return false;

      if (minor < rhs.minor) return true;
      if (minor > rhs.minor) return false;

      if (point < rhs.point) return true;
      if (point > rhs.point) return false;

      return false;
    }

    bool operator==(const Version& rhs) const
    {
      return major == rhs.major &&
             minor == rhs.minor &&
             point == rhs.point;
    }

    bool operator<=(const Version& rhs) const { return  operator<(rhs) ||  operator==(rhs); }
    bool operator>(const Version& rhs) const  { return !operator<(rhs) && !operator==(rhs); }
    bool operator>=(const Version& rhs) const { return !operator<(rhs); }

    unsigned int major;
    unsigned int minor;
    unsigned int point;
  };
}

// --- CServer -----------------------------------------------------------------

CServer::CServer(void) :
    m_socket(NULL),
    m_readMutex(new CMutex),
    m_bConnectionLost(true)
{
}

CServer::~CServer(void)
{
  Close();
  delete m_readMutex;
}

bool CServer::Open(const std::string& hostname, unsigned int port)
{
  uint64_t iNow = GetTimeMs();
  uint64_t iTarget = iNow + CONNECTION_TIMEOUT_MS;

  if (!m_socket)
    m_socket = new CTcpConnection(hostname.c_str(), port);

  while (!m_socket->IsOpen() && iNow < iTarget)
  {
    if (!m_socket->Open(iTarget - iNow))
      CEvent::Sleep(100);

    iNow = GetTimeMs();
  }

  if (!m_socket->IsOpen())
  {
    //XBMC->Log(LOG_ERROR, "%s - failed to connect to the backend (%s)", __FUNCTION__, m_socket->GetError().c_str()); // TODO
    return false;
  }

  return true;
}

bool CServer::IsOpen(void)
{
  bool bReturn(false);

  if (m_socket && m_socket->IsOpen())
    bReturn = true;

  return bReturn;
}

void CServer::Close(void)
{
  if (IsOpen())
    m_socket->Close();

  if (m_socket)
  {
    delete m_socket;
    m_socket = NULL;
  }
}

bool CServer::Login(void)
{
  std::string strRequest;
  std::string strResponse;
  if (Send(RPC_METHOD::Login, strRequest, strResponse))
  {
    try
    {
      /* TODO
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

      //XBMC->Log(LOG_NOTICE, "Logged in at '%lu + %i' to '%s' Version: '%s' with protocol version '%d'", vdrTime, vdrTimeOffset, ServerName, ServerVersion, protocol); // TODO
      */
      throw "Not implemented";
    }
    catch (const char* strError)
    {
      //XBMC->Log(LOG_ERROR, "%s - %s", __FUNCTION__, strError); // TODO
      if (m_socket)
      {
        m_socket->Close();
        delete m_socket;
        m_socket = NULL;
      }
      return false;
    }
  }

  return true;
}

bool CServer::Send(RPC_METHOD method, const std::string& request)
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

bool CServer::Send(RPC_METHOD method, const std::string& request, std::string& response)
{
  if (!Send(method, request))
    return false;;

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

bool CServer::SendHeader(RPC_METHOD method, size_t msgLength)
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

bool CServer::SendData(const std::string& request)
{
  ssize_t iWriteResult = m_socket->Write(const_cast<char*>(request.c_str()), request.length());
  if (iWriteResult != (ssize_t)request.length())
  {
    //XBMC->Log(LOG_ERROR, "%s - Failed to write packet (%s), bytes written: %d of total: %d", __FUNCTION__, m_socket->GetError().c_str(), iWriteResult, vrp->getLen()); // TODO
    return false;
  }
  return true;
}

bool CServer::ReadMessage(RPC_METHOD method,
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

bool CServer::ReadHeader(RPC_METHOD& method, size_t& length, unsigned int timeoutMs)
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

bool CServer::ReadData(std::string& buffer, size_t totalBytes, unsigned int timeoutMs)
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

void CServer::SignalConnectionLost(void)
{
  if (m_bConnectionLost)
    return;

  //XBMC->Log(LOG_ERROR, "%s - connection lost !!!", __FUNCTION__); // TODO

  m_bConnectionLost = true;
  Close();

  OnDisconnect();
}
