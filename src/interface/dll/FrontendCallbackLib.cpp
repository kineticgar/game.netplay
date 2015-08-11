/*
 *      Copyright (C) 2015 Garrett Brown
 *      Copyright (C) 2015 Team XBMC
 *
 *  This Program is free software you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "FrontendCallbackLib.h"
#include "FrontendCallbacks.h"
#include "interface/IFrontend.h"
#include "log/Log.h"

#include "kodi/kodi_game_callbacks.h"

using namespace NETPLAY;

CFrontendCallbackLib::CFrontendCallbackLib(IFrontend* frontend, const std::string& strLibDir) :
  m_frontend(frontend),
  m_callbacks(new AddonCB),
  m_helperAddon(NULL),
  m_helperGame(NULL)
{
  m_callbacks->libBasePath        = new char[strLibDir.length() + 1];
  m_callbacks->addonData          = this;
  m_callbacks->RegisterAddonLib   = CFrontendCallbackLib::RegisterAddonLib;
  m_callbacks->UnregisterAddonLib = CFrontendCallbackLib::UnregisterAddonLib;
  m_callbacks->RegisterGameLib    = CFrontendCallbackLib::RegisterGameLib;
  m_callbacks->UnregisterGameLib  = CFrontendCallbackLib::UnregisterGameLib;

  std::strcpy(m_callbacks->libBasePath, strLibDir.c_str());
}

CFrontendCallbackLib::~CFrontendCallbackLib(void)
{
  delete   m_helperAddon;
  delete   m_helperGame;
  delete[] m_callbacks->libBasePath;
  delete   m_callbacks;
}

CB_AddOnLib* CFrontendCallbackLib::RegisterAddonLib(CFrontendCallbackLib* addon)
{
  if (addon == NULL)
  {
    esyslog("%s - called with a null pointer", __FUNCTION__);
    return NULL;
  }

  addon->m_helperAddon = new CFrontendCallbacksAddon(addon->m_frontend);

  return addon->m_helperAddon->GetCallbacks();
}

void CFrontendCallbackLib::UnregisterAddonLib(CFrontendCallbackLib* addon, CB_AddOnLib* cbTable)
{
  if (addon == NULL)
  {
    esyslog("%s - called with a null pointer", __FUNCTION__);
    return;
  }

  delete addon->m_helperAddon;
  addon->m_helperAddon = NULL;
}

CB_GameLib* CFrontendCallbackLib::RegisterGameLib(CFrontendCallbackLib* addon)
{
  if (addon == NULL)
  {
    esyslog("%s - called with a null pointer", __FUNCTION__);
    return NULL;
  }

  addon->m_helperGame = new CFrontendCallbacksGame(addon->m_frontend);
  return addon->m_helperGame->GetCallbacks();
}

void CFrontendCallbackLib::UnregisterGameLib(CFrontendCallbackLib* addon, CB_GameLib *cbTable)
{
  if (addon == NULL)
  {
    esyslog("%s - called with a null pointer", __FUNCTION__);
    return;
  }

  delete addon->m_helperGame;
  addon->m_helperGame = NULL;
}
