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

#include "Network.h"

#include <signal.h>

using namespace NETPLAY;

CNetwork& CNetwork::Get(void)
{
  static CNetwork instance;
  return instance;
}

bool CNetwork::Initialize(void)
{
#ifdef _WIN32
   WSADATA wsaData;
#endif

#if defined(_WIN32)
   if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
   {
      network_deinit();
      return false;
   }
#else
   signal(SIGPIPE, SIG_IGN); /* Do not like SIGPIPE killing our app. */
#endif

   return true;
}

void CNetwork::Deiitialize(void)
{
#if defined(_WIN32)
   WSACleanup();
#endif
}
