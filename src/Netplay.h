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

#include "platform/threads/mutex.h"

#include <stddef.h>
#include <vector>

namespace NETPLAY
{
  class IFrontend;
  class IGame;

  class CNetplay
  {
  public:
    CNetplay(void);
    ~CNetplay(void) { Deinitialize(); }

    bool Initialize(void);
    void Deinitialize(void);

    void RegisterGame(IGame* game);
    void UnregisterGame(void);

    void RegisterFrontend(IFrontend* frontend);
    void UnregisterFrontend(IFrontend* frontend);

  private:
    std::vector<IFrontend*> m_frontends;
    IGame*                  m_game;
    PLATFORM::CMutex        m_mutex;
  };
}
