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

#include "interface/IGame.h"

#include <string>

namespace NETPLAY
{
  class CGameDLL : public IGame
  {
  public:
    CGameDLL(const std::string& strDllPath);
    virtual ~CGameDLL(void) { Deinitialize(); }

    virtual bool Initialize(void);
    virtual void Deinitialize(void);

  private:
    const std::string m_strPath;
  };
}
