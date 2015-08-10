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

#include "KeyboardAddon.h"

#include "kodi/libKODI_guilib.h"

#include <assert.h>

using namespace NETPLAY;

CKeyboardAddon::CKeyboardAddon(CHelper_libKODI_guilib* gui) :
  m_gui(gui)
{
  assert(m_gui);
}

bool CKeyboardAddon::PromptForInput(const std::string& strPrompt, std::string& strInput)
{
  char input[1024] = { };
  if (m_gui->Dialog_Keyboard_ShowAndGetInput(*input, sizeof(input) - 1, strPrompt.c_str(), false, false))
  {
    strInput = input;
    return true;
  }

  return false;
}
