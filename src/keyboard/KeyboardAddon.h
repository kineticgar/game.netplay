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

#include "IKeyboard.h"

class CHelper_libKODI_guilib;

namespace NETPLAY
{
  class CKeyboardAddon : public IKeyboard
  {
  public:
    CKeyboardAddon(CHelper_libKODI_guilib* gui);
    virtual ~CKeyboardAddon(void) { }

    // implementation of IKeyboard
    virtual bool PromptForInput(const std::string& strPrompt, std::string& strInput);
    virtual SYS_KEYBOARD_TYPE Type(void) const { return SYS_KEYBOARD_TYPE_ADDON; }

  private:
    CHelper_libKODI_guilib* const m_gui;
  };
}
