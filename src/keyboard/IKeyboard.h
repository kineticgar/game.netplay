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

#include <string>

namespace NETPLAY
{
  enum SYS_KEYBOARD_TYPE
  {
    SYS_KEYBOARD_TYPE_NONE,     // Input is disabled
    SYS_KEYBOARD_TYPE_CONSOLE,  // Get input from stdin
    SYS_KEYBOARD_TYPE_ADDON,    // Get input from the frontend's GUI lib
    SYS_KEYBOARD_TYPE_MOCK,     // Parrot back pre-recorded input
  };

  /*!
   * \brief Keyboard interface
   */
  class IKeyboard
  {
  public:
    virtual ~IKeyboard(void) { }

    /*!
     * \brief Prompt the user for input
     * \param strPrompt the prompt to show the user while asking for input
     * \param strInput the user's input, or empty if the prompt is cancelled
     * \return true if strInput is not empty
     */
    virtual bool PromptForInput(const std::string& strPrompt, std::string& strInput) = 0;

    /*!
     * \brief Get the type of keyboard used in the implementation
     */
    virtual SYS_KEYBOARD_TYPE Type(void) const = 0;
  };
}
