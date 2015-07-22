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

using namespace NETPLAY;
using namespace PLATFORM;

#define CONNECTION_TIMEOUT_MS  2000

CClient::CClient(const std::string& strAddress, unsigned int port) :
  m_strAddress(strAddress),
  m_port(port),
  m_socket(NULL)
{
}

CClient::~CClient(void)
{
  Close();
}

bool CClient::Open(void)
{
  Close();

  isyslog("Connecting to %s", Address().c_str());

  uint64_t iNow = GetTimeMs();
  uint64_t iTarget = iNow + CONNECTION_TIMEOUT_MS;

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
  {
    isyslog("Closing connection to %s", Address().c_str());
    m_socket->Close();
  }

  if (m_socket)
  {
    delete m_socket;
    m_socket = NULL;
  }
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

bool CClient::ReadData(std::string& buffer, size_t totalBytes, unsigned int timeoutMs)
{
  if (totalBytes == 0)
    return false;

  buffer.resize(totalBytes);

  unsigned int bytesRead = m_socket->Read(const_cast<char*>(buffer.c_str()), totalBytes, timeoutMs);

  if (m_socket->GetErrorNumber() == ETIMEDOUT)
  {
    if (0 < bytesRead && bytesRead < totalBytes)
    {
      // We read something, so try to finish the read
      unsigned int bytes = m_socket->Read(const_cast<char*>(buffer.c_str()) + bytesRead,
                                          totalBytes - bytesRead,
                                          timeoutMs);
      if (bytes > 0)
        bytesRead += bytes;
    }
    else
    {
      SignalConnectionLost();
    }
  }

  return bytesRead == totalBytes;
}

std::string CClient::Address(void) const
{
  return StringUtils::Format("%s:%d", m_strAddress.c_str(), m_port);
}
