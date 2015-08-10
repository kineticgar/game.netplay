/*
 *      Copyright (C) 2014-2015 Garrett Brown
 *      Copyright (C) 2014-2015 Team XBMC
 *      Portions Copyright (C) 2013-2014 Lars Op den Kamp
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

#include "Keyboard.h"
#include "KeyboardAddon.h"
#include "KeyboardConsole.h"
#include "log/Log.h"

using namespace NETPLAY;
using namespace PLATFORM;

CKeyboard::CKeyboard(IKeyboard* pipe) :
  m_pipe(pipe)
{
}

CKeyboard& CKeyboard::Get(void)
{
  static CKeyboard _instance(new CKeyboardConsole);
  return _instance;
}

CKeyboard::~CKeyboard(void)
{
  SetPipe(NULL);
}

bool CKeyboard::SetType(SYS_KEYBOARD_TYPE type)
{
  PLATFORM::CLockObject lock(m_mutex);

  if (m_pipe && m_pipe->Type() == type)
    return true; // Already set

  switch (type)
  {
  case SYS_KEYBOARD_TYPE_CONSOLE:
    SetPipe(new CKeyboardConsole);
    return true;
  case SYS_KEYBOARD_TYPE_NONE:
    SetPipe(NULL);
    return true;
  case SYS_KEYBOARD_TYPE_ADDON: // No default constructor
  case SYS_KEYBOARD_TYPE_MOCK:  // No default constructor
  default:
    esyslog("Failed to set log type to %s", TypeToString(type));
  }

  return false;
}

void CKeyboard::SetPipe(IKeyboard* pipe)
{
  PLATFORM::CLockObject lock(m_mutex);

  delete m_pipe;
  m_pipe = pipe;
}

bool CKeyboard::PromptForInput(const std::string& strPrompt, std::string& strInput)
{
  PLATFORM::CLockObject lock(m_mutex);

  if (m_pipe)
    return m_pipe->PromptForInput(strPrompt, strInput);

  return false;
}

const char* CKeyboard::TypeToString(SYS_KEYBOARD_TYPE type)
{
  switch (type)
  {
  case SYS_KEYBOARD_TYPE_NONE:
    return "none";
  case SYS_KEYBOARD_TYPE_CONSOLE:
    return "console";
  case SYS_KEYBOARD_TYPE_ADDON:
    return "addon";
  default:
    return "unknown";
  }
}
