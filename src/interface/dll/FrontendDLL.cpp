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

#include "FrontendDLL.h"

#include "kodi/libKODI_game.h"
#include "kodi/libXBMC_addon.h"

using namespace ADDON;
using namespace NETPLAY;

CFrontendDLL::CFrontendDLL(void* callbacks)
{
  try
  {
    m_addon = new CHelper_libXBMC_addon;
    if (!m_addon->RegisterMe(callbacks))
      throw false;

    m_game = new CHelper_libKODI_game;
    if (!m_game->RegisterMe(callbacks))
      throw false;
  }
  catch (const bool& bSuccess)
  {
    if (!bSuccess)
      Deinitialize();
  }
}

bool CFrontendDLL::Initialize(void)
{
  return m_addon != NULL &&
         m_game  != NULL;
}

void CFrontendDLL::Deinitialize(void)
{
  delete m_addon;
  delete m_game;

  m_addon = NULL;
  m_game  = NULL;
}
