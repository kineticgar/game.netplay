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

#include "kodi/kodi_addon_callbacks.h"

#include <cstring>
#include <string>

struct CB_GameLib;

namespace NETPLAY
{
  class CFrontendCallbackLib;

  struct AddonCB
  {
    char*                 libBasePath;  ///> Never, never change this!!!
    CFrontendCallbackLib* addonData;

    CB_AddOnLib* (*RegisterAddonLib)(CFrontendCallbackLib* addon);
    void         (*UnregisterAddonLib)(CFrontendCallbackLib* addon, CB_AddOnLib* cbTable);
    CB_GameLib*  (*RegisterGameLib)(CFrontendCallbackLib* addon);
    void         (*UnregisterGameLib)(CFrontendCallbackLib* addon, CB_GameLib* cbTable);
  };

  class CFrontendCallbacksAddon;
  class CFrontendCallbacksGame;
  class IFrontend;

  class CFrontendCallbackLib
  {
  public:
    CFrontendCallbackLib(IFrontend* frontend, const std::string& strHelperDir);
    ~CFrontendCallbackLib(void);

    AddonCB* GetCallbacks(void) { return m_callbacks; }

    static CB_AddOnLib* RegisterAddonLib(CFrontendCallbackLib* addon);
    static void         UnregisterAddonLib(CFrontendCallbackLib* addon, CB_AddOnLib* cbTable);

    static CB_GameLib* RegisterGameLib(CFrontendCallbackLib* addon);
    static void        UnregisterGameLib(CFrontendCallbackLib* addon, CB_GameLib* cbTable);

    CFrontendCallbacksAddon* GetHelperAddon(void) { return m_helperAddon; }
    CFrontendCallbacksGame*  GetHelperGame(void)  { return m_helperGame; }

  private:
    IFrontend* const         m_frontend;
    AddonCB*                 m_callbacks;
    CFrontendCallbacksAddon* m_helperAddon;
    CFrontendCallbacksGame*  m_helperGame;
  };
}
