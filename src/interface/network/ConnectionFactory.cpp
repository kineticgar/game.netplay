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

#include "ConnectionFactory.h"
#include "interface/network/Client.h"
#include "interface/network/linux/LinuxConnection.h"

using namespace NETPLAY;

IConnection* CConnectionFactory::CreateConnection(int fd)
{
  return new CLinuxConnection(fd);
}

IConnection* CConnectionFactory::CreateConnection(const std::string& strAddress, unsigned int port)
{
  return new CClient(strAddress, port);
}
