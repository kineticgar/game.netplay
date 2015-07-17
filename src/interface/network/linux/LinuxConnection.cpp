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

#include "LinuxConnection.h"
#include "log/Log.h"

#include <cstdio>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <stdint.h>

using namespace NETPLAY;

// --- ip2txt ------------------------------------------------------------------

namespace NETPLAY
{
  std::string ip2txt(uint32_t ip, unsigned int port)
  {
    // inet_ntoa is not thread-safe (?)
    unsigned int iph = static_cast<unsigned int>(ntohl(ip));
    unsigned int porth = static_cast<unsigned int>(ntohs(port));

    char str[64];

    if (porth == 0)
    {
      std::sprintf(str, "%d.%d.%d.%d", ((iph >> 24) & 0xff),
                                       ((iph >> 16) & 0xff),
                                       ((iph >> 8)  & 0xff),
                                       ((iph)       & 0xff));
    }
    else
    {
      std::sprintf(str, "%u.%u.%u.%u:%u", ((iph >> 24) & 0xff),
                                          ((iph >> 16) & 0xff),
                                          ((iph >> 8)  & 0xff),
                                          ((iph)       & 0xff),
                                          porth);
    }

    return str;
  }
}

// --- CLinuxConnection --------------------------------------------------------

CLinuxConnection::CLinuxConnection(int fd) :
  m_fd(fd),
  m_bConnectionLost(true)
{
  m_socket.SetHandle(m_fd);
}

bool CLinuxConnection::Open(void)
{
  struct sockaddr_in sin;
  socklen_t len = sizeof(sin);

  if (getpeername(m_fd, reinterpret_cast<struct sockaddr*>(&sin), &len))
  {
    esyslog("%s - getpeername() failed, dropping new incoming connection", __FUNCTION__);
    close(m_fd);
    return false;
  }

  if (fcntl(m_fd, F_SETFL, fcntl (m_fd, F_GETFL) | O_NONBLOCK) == -1)
  {
    esyslog("%s - Error setting control socket to nonblocking mode", __FUNCTION__);
    return false;
  }

  int val = 1;
  setsockopt(m_fd, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(val));

  val = 30;
  setsockopt(m_fd, SOL_TCP, TCP_KEEPIDLE, &val, sizeof(val));

  val = 15;
  setsockopt(m_fd, SOL_TCP, TCP_KEEPINTVL, &val, sizeof(val));

  val = 5;
  setsockopt(m_fd, SOL_TCP, TCP_KEEPCNT, &val, sizeof(val));

  val = 1;
  setsockopt(m_fd, SOL_TCP, TCP_NODELAY, &val, sizeof(val));

  m_strClientAddress = ip2txt(sin.sin_addr.s_addr, sin.sin_port);

  isyslog("Client connected: %s", m_strClientAddress.c_str());

  return CreateThread();
}

void CLinuxConnection::Close(void)
{
  StopThread(0);

  m_socket.Close();
}

void* CLinuxConnection::Process(void)
{
  uint32_t channelID;
  uint32_t dataLength;
  uint8_t* data;

  while (!IsStopped())
  {
    if (!m_socket.Read(reinterpret_cast<uint8_t*>(&channelID), sizeof(uint32_t)))
      break;

    channelID = ntohl(channelID);

    if (channelID == 1)
    {
      if (!m_socket.Read((uint8_t*)&dataLength, sizeof(uint32_t), 10000))
        break;

      dataLength = ntohl(dataLength);

      if (dataLength > 200000) // a random sanity limit
      {
        esyslog("%s - dataLength > 200000!", __FUNCTION__);
        break;
      }

      if (dataLength)
      {
        data = new uint8_t[dataLength];
        if (!data)
        {
          esyslog("%s - Extra data buffer malloc error", __FUNCTION__);
          break;
        }

        if (!m_socket.Read(data, dataLength, 10000))
        {
          esyslog("%s - Could not read data", __FUNCTION__);
          delete[] data;
          break;
        }
      }
      else
      {
        data = NULL;
      }

      /*
      if (!m_loggedIn && (opcode != VNSI_LOGIN))
      {
        esyslog("%s - Clients must be logged in before sending commands! Aborting", __FUNCTION__);
        if (data) free(data);
        break;
      }
      */

      /* TODO
      cRequestPacket* req = new cRequestPacket(requestID, opcode, data, dataLength);

      processRequest(req);

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
    else
    {
      esyslog("%s - Incoming channel number unknown", __FUNCTION__);
      break;
    }
  }

  return NULL;
}
