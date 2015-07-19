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

#include "RPCMethods.h"
#include "utils/Observer.h"

#include <string>

namespace NETPLAY
{
  class IConnection : public Observable
  {
  public:
    virtual ~IConnection(void) { }

    virtual std::string Address(void) const = 0;

    virtual bool Open(void) = 0;
    virtual bool IsOpen(void) = 0;
    virtual void Close(void) = 0;

    virtual bool Send(RPC_METHOD method, const std::string& request) = 0;
    virtual bool Send(RPC_METHOD method, const std::string& request, std::string& response) = 0;
  };
}
