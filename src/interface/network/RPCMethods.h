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

namespace NETPLAY
{
  enum class RPC_METHOD
  {
    Invalid = 0, // Message is invalid

    // --- Add-on methods ------------------------------------------------------

    Login,
    Logout,
    GetStatus,
    SetSetting,
    Announce,

    // --- Add-on callbacks ----------------------------------------------------

    Log,
    GetSetting,
    QueueNotification,
    WakeOnLan,
    UnknownToUTF8,
    GetLocalizedString,
    GetDVDMenuLanguage,

    // --- Game methods --------------------------------------------------------

    GetGameAPIVersion,
    GetMininumGameAPIVersion,
    LoadGame,
    LoadGameSpecial,
    LoadStandalone,
    UnloadGame,
    GetGameInfo,
    GetRegion,
    FrameEvent,
    Reset,
    UpdatePort,
    InputEvent,
    SerializeSize,
    Serialize,
    Deserialize,
    CheatReset,
    GetMemory,
    SetCheat,

    // --- Game callbacks ------------------------------------------------------

    CloseGame,
    VideoFrame,
    AudioFrames,
    OpenPort,
    ClosePort,
    RumbleSetState,
    StartLocation,
    StopLocation,
    GetLocation,
    SetLocationInterval,

    RPC_METHOD_COUNT,
  };
}
