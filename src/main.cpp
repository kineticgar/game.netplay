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

#include "interface/dll/DLLGame.h"
#include "interface/FrontendManager.h"
#include "interface/network/NetworkGame.h"
#include "interface/network/Server.h"
#include "utils/PathUtils.h"
#include "utils/StringUtils.h"

#include <iostream>
#include <string>

using namespace NETPLAY;

// --- Helper functions --------------------------------------------------------

namespace NETPLAY
{
  /*
  ADDON_STATUS_OK,
  ADDON_STATUS_LOST_CONNECTION,
  ADDON_STATUS_NEED_RESTART,
  ADDON_STATUS_NEED_SETTINGS,
  ADDON_STATUS_UNKNOWN,
  ADDON_STATUS_NEED_SAVEDSETTINGS,
  ADDON_STATUS_PERMANENT_FAILURE,
  */
}

// --- Entry point -------------------------------------------------------------

enum OPTION
{
  OPTION_INVALID,
  OPTION_GAME_CLIENT, // Load game client
  OPTION_REMOTE_GAME, // Load remote game client
  OPTION_DISCOVER,    // Discover servers on the network
};

int main(int argc, char** argv)
{
  CFrontendManager* CALLBACKS = NULL;
  IGame*            GAME      = NULL;
  CServer*          SERVER    = NULL;

  OPTION option(OPTION_INVALID);

  if (argc >= 2)
  {
    std::string strOption = argv[1];

    if ((strOption == "-g" || strOption == "--game") && (argc == 6 || argc == 7))
      option = OPTION_GAME_CLIENT;
    else if ((strOption == "-r" || strOption == "--remote") && argc == 4)
      option = OPTION_REMOTE_GAME;
    else if ((strOption == "-d" || strOption == "--discover") && argc ==  2)
      option = OPTION_DISCOVER;
  }

  if (option == OPTION_INVALID)
  {
    std::string strExe = PathUtils::GetFileName(PathUtils::GetProcessPath());

    std::cout << "Load game client:" << std::endl;
    std::cout << "  " << strExe << " --game <DLL> <system dir> <content dir> <save dir>" << std::endl;
    std::cout << std::endl;
    std::cout << "Load game client via proxy DLL:" << std::endl;
    std::cout << "  " << strExe << " --game <proxy DLL> <DLL> <system dir> <content dir> <save dir>" << std::endl;
    std::cout << std::endl;
    std::cout << "Load remote game client" << std::endl;
    std::cout << "  " << strExe << " --remote <address> <port>" << std::endl;
    std::cout << std::endl;
    std::cout << "Discover servers on the network:" << std::endl;
    std::cout << "  " << strExe << " --discover" << std::endl;
    std::cout << std::endl;
    return 1;
  }

  CALLBACKS = new CFrontendManager;
  if (!CALLBACKS->Initialize())
  {
    std::cerr << "Failed to initialize frontend" << std::endl;
    return 2;
  }

  switch (option)
  {
    case OPTION_GAME_CLIENT:
    {
      game_client_properties props = { };
      if (argc == 6)
      {
        props.game_client_dll_path = argv[2];
        props.proxy_dll_count      = 0;
        props.netplay_server       = "";
        props.system_directory     = argv[3];
        props.content_directory    = argv[4];
        props.save_directory       = argv[5];
      }
      else
      {
        const char* proxyDll       = argv[2];
        props.proxy_dll_paths      = &proxyDll;
        props.proxy_dll_count      = 1;
        props.game_client_dll_path = argv[3];
        props.proxy_dll_count      = 0;
        props.netplay_server       = "";
        props.system_directory     = argv[4];
        props.content_directory    = argv[5];
        props.save_directory       = argv[6];
      }

      std::string strLibBasePath = PathUtils::GetHelperLibraryDir(PathUtils::GetParentDirectory(PathUtils::GetProcessPath()));
      GAME = new CDLLGame(CALLBACKS, props, strLibBasePath);
      break;
    }
    case OPTION_REMOTE_GAME:
    {
      GAME = new CNetworkGame(argv[2], StringUtils::IntVal(argv[3]));
      break;
    }
    case OPTION_DISCOVER:
    {
      // TODO
      std::cout << "Network discovery is unimplemented" << std::endl;
      return 3;
    }
    default:
      break;
  }

  if (!GAME)
  {
    std::cout << "Server failed to connect to a game client. Call with no args for help." << std::endl;
    return 4;
  }

  ADDON_STATUS status = GAME->Initialize();
  {
    std::cerr << "Failed to load game add-on" << std::endl;
    return 4;
  }

  SERVER = new CServer(GAME, CALLBACKS);
  if (!SERVER->Initialize())
  {
    std::cerr << "Failed to initialize server" << std::endl;
    return 5;
  }

  SERVER->WaitForExit();

  delete SERVER;
  delete CALLBACKS;
  delete GAME;

  return 0;
}
