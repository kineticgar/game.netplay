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

#include "interface/IFrontend.h"

namespace ADDON { class CHelper_libXBMC_addon; }
class CHelper_libKODI_game;

namespace NETPLAY
{
  class CFrontendDLL : public IFrontend
  {
  public:
    CFrontendDLL(void* callbacks);
    virtual ~CFrontendDLL(void) { Deinitialize(); }

    virtual bool Initialize(void);
    virtual void Deinitialize(void);

  private:
    ADDON::CHelper_libXBMC_addon* m_addon;
    CHelper_libKODI_game*         m_game;
  };
}
