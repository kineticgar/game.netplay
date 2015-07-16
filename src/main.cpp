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
#include "interface/network/AbortableServer.h"
#include "interface/network/NetworkGame.h"
#include "log/Log.h"
#include "utils/PathUtils.h"
#include "utils/StringUtils.h"

#include "kodi/kodi_addon_utils.hpp"

#include <iostream>
#include <signal.h>
#include <stdexcept>
#include <string>

using namespace NETPLAY;

enum OPTION
{
  OPTION_INVALID,
  OPTION_GAME_CLIENT, // Load game client
  OPTION_REMOTE_GAME, // Load remote game client
  OPTION_DISCOVER,    // Discover servers on the network
};

// --- Helper function --------------------------------------------------------

namespace NETPLAY
{
  IGame* GetGame(OPTION option, int argc, char* argv[], IFrontend* callbacks)
  {
    IGame* game = NULL;

    switch (option)
    {
      case OPTION_GAME_CLIENT:
      {
        GameClientProperties props;
        if (argc == 6)
        {
          props.game_client_dll_path = argv[2];
          props.system_directory     = argv[3];
          props.content_directory    = argv[4];
          props.save_directory       = argv[5];
        }
        else
        {
          props.proxy_dll_paths.push_back(argv[2]);
          props.game_client_dll_path = argv[3];
          props.system_directory     = argv[4];
          props.content_directory    = argv[5];
          props.save_directory       = argv[6];
        }

        std::string strLibBasePath = PathUtils::GetHelperLibraryDir(PathUtils::GetParentDirectory(PathUtils::GetProcessPath()));
        game = new CDLLGame(callbacks, props, strLibBasePath);
        break;
      }
      case OPTION_REMOTE_GAME:
      {
        game = new CNetworkGame(argv[2], StringUtils::IntVal(argv[3]));
        break;
      }
      case OPTION_DISCOVER:
      {
        // TODO
        std::cout << "Network discovery is unimplemented" << std::endl;
        break;
      }
      default:
        break;
    }

    return game;
  }
}

// --- Entry point -------------------------------------------------------------

int main(int argc, char* argv[])
{
  CFrontendManager* CALLBACKS = NULL;
  IGame*            GAME      = NULL;
  CAbortableServer* SERVER    = NULL;

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

  try
  {
    CALLBACKS = new CFrontendManager;
    if (!CALLBACKS->Initialize())
      throw "Failed to initialize frontend";

    GAME = GetGame(option, argc, argv, CALLBACKS);
    if (!GAME)
      throw std::runtime_error("Server failed to connect to a game client. Call with no args for help.");

    ADDON_STATUS status = GAME->Initialize();
    if (status == ADDON_STATUS_UNKNOWN ||status == ADDON_STATUS_PERMANENT_FAILURE)
      throw std::runtime_error("Failed to initialize game client");

    SERVER = new CAbortableServer(GAME, CALLBACKS);
    if (!SERVER->Initialize())
      throw std::runtime_error("Failed to initialize server");
  }
  catch (const std::runtime_error& error)
  {
    esyslog("%s", error.what());
    return 1;
  }

  CSignalHandler::Get().SetSignalReceiver(SIGHUP, SERVER);
  CSignalHandler::Get().SetSignalReceiver(SIGINT, SERVER);
  CSignalHandler::Get().SetSignalReceiver(SIGKILL, SERVER);
  CSignalHandler::Get().SetSignalReceiver(SIGTERM, SERVER);
  CSignalHandler::Get().IgnoreSignal(SIGPIPE);

  SERVER->WaitForExit();

  delete SERVER;
  delete CALLBACKS;
  delete GAME;

  return SERVER->GetReturnCode();
}
