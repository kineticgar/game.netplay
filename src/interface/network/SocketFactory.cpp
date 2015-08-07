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

#include "SocketFactory.h"
#include "interface/network/linux/LinuxServerSocket.h"
#include "interface/network/platform/PlatformSocket.h"

using namespace NETPLAY;

ServerSocketPtr CSocketFactory::CreateServerSocket(void)
{
  ServerSocketPtr socket;

#if !defined(_WIN32) // TODO
  socket = ServerSocketPtr(new CLinuxServerSocket);
#endif

  return socket;
}

SocketPtr CSocketFactory::CreateClientSocket(const std::string& strAddress, unsigned int port)
{
  SocketPtr socket;

  socket = SocketPtr(new CPlatformSocket(strAddress, port));

  return socket;
}
