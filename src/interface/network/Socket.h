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

#include "ISocket.h"

#include <stddef.h>
#include <stdint.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>

struct addrinfo;

namespace NETPLAY
{
  class CSocket : public ISocket
  {
  public:
    CSocket(const std::string& strAddress, uint16_t port, SOCKET_TYPE type);
    CSocket(uint16_t port, SOCKET_TYPE type);

    virtual ~CSocket(void) { Shutdown(); }

    // implementation of ISocket
    virtual bool Connect(void);
    virtual void Shutdown(void);
    virtual bool Read(std::string& buffer, unsigned int totalBytes) { return false; } // TODO
    virtual bool Write(const std::string& request) { return false; } // TODO

    bool nonblock(void);

    int send_all_blocking(const void *data_, size_t size);

    int receive_all_blocking(uint8_t *data, size_t size);

    bool SendTo(const uint8_t* buf, size_t size, int flags, const struct sockaddr* addr, unsigned int addr_len);

    bool ReceiveFrom(uint8_t* buf, size_t size, int flags, struct sockaddr* addr, unsigned int *addr_len);

    CSocket* Accept(struct sockaddr* addr, unsigned int* addr_size);

    void Set(fd_set* fds) const;

    bool IsSet(fd_set* fds) const;

    void SetSendBuffer(int bufsize);

    int max_fd(const CSocket* other_socket) const;

    static int select_sockets(int nfds, fd_set *readfs, fd_set *writefds, fd_set *errorfds, struct timeval *timeout);

  private:
    void ConnectTCP(const addrinfo* res);
    void ConnectUDP(const addrinfo* res);

    addrinfo GetHints(void) const;

    bool        m_bServer;
    std::string m_strAddress;
    uint16_t    m_port;
    SOCKET_TYPE m_type;
    int         m_fd;
  };
}
