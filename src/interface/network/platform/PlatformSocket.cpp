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

#include "PlatformSocket.h"
#include "log/Log.h"
#include "utils/StringUtils.h"

using namespace NETPLAY;
using namespace PLATFORM;

#define CONNECTION_TIMEOUT_MS  2000
#define READ_TIMEOUT_MS        5000

CPlatformSocket::CPlatformSocket(const std::string& strAddress, unsigned int port) :
  m_strAddress(strAddress),
  m_port(port),
  m_socket(NULL)
{
}

bool CPlatformSocket::Connect(void)
{
  Shutdown();

  CLockObject lock(m_abortMutex);
  CLockObject lock2(m_socketMutex);

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
    esyslog("Failed to connect to the server (%s)", m_socket->GetError().c_str());
    delete m_socket;
    m_socket = NULL;
    return false;
  }

  return true;
}

void CPlatformSocket::Shutdown(void)
{
  AbortRead();

  CLockObject lock(m_abortMutex);
  CLockObject lock2(m_socketMutex);

  if (m_socket && m_socket->IsOpen())
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

bool CPlatformSocket::Read(std::string& buffer, unsigned int totalBytes)
{
  if (totalBytes == 0)
    return false;

  buffer.resize(totalBytes);

  unsigned int totalBytesRead = 0;

  while (totalBytesRead < totalBytes)
  {
    bool bInterrupted = false;

    {
      CLockObject lock(m_socketMutex);

      if (!m_socket)
        return false;

      ssize_t bytesRead = m_socket->Read(const_cast<char*>(buffer.data()) + totalBytesRead,
                                         totalBytes - totalBytesRead,
                                         READ_TIMEOUT_MS);

      if (bytesRead > 0)
        totalBytesRead += bytesRead;

      if (m_socket->GetErrorNumber() == ETIMEDOUT)
      {
        esyslog("Socket timed out, read %u of %u bytes", totalBytesRead, totalBytes);
        break;
      }
      else if (m_socket->GetErrorNumber() == ECONNRESET)
      {
        esyslog("Socket closed, connection reset by peer");
        break;
      }
      else if (m_socket->GetErrorNumber() == EINTR)
      {
        bInterrupted = true;
      }
    }

    if (bInterrupted)
    {
      // Read was interrupted, try again, but not too soon
      CEvent::Sleep(1);
    }
  }

  return totalBytesRead == totalBytes;
}

void CPlatformSocket::AbortRead(void)
{
  CLockObject lock(m_abortMutex);
  if (m_socket)
    m_socket->AbortRead();
}

bool CPlatformSocket::Write(const std::string& request)
{
  AbortRead();

  CLockObject lock(m_socketMutex);

  if (!m_socket)
    return false;

  if (!request.empty())
  {
    ssize_t iWriteResult = m_socket->Write(const_cast<char*>(request.data()), request.length());
    if (iWriteResult != static_cast<ssize_t>(request.length()))
    {
      esyslog(" Failed to write packet (%s), bytes written: %d of total: %d", m_socket->GetError().c_str(), iWriteResult, request.length());
      return false;
    }
  }

  return true;
}

std::string CPlatformSocket::Address(void) const
{
  return StringUtils::Format("%s:%d", m_strAddress.c_str(), m_port);
}
