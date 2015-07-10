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

#include "Netplay.h"

#include "kodi/libKODI_game.h"
#include "kodi/libXBMC_addon.h"

#include <algorithm>

using namespace NETPLAY;
using namespace PLATFORM;

CNetplay::CNetplay(void) :
  m_game(NULL)
{
}

bool CNetplay::Initialize(void)
{
  return true;
}

void CNetplay::Deinitialize(void)
{
}

void CNetplay::RegisterGame(IGame* game)
{
  CLockObject lock(m_mutex);
  m_game = game;
}

void CNetplay::UnregisterGame(void)
{
  CLockObject lock(m_mutex);
  m_game = NULL;
}

void CNetplay::RegisterFrontend(IFrontend* frontend)
{
  CLockObject lock(m_mutex);
  m_frontends.push_back(frontend);
}

void CNetplay::UnregisterFrontend(IFrontend* frontend)
{
  CLockObject lock(m_mutex);
  m_frontends.erase(std::remove(m_frontends.begin(), m_frontends.end(), frontend), m_frontends.end());
}
