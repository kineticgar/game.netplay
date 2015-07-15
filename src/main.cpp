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
#include "utils/StringUtils.h"

#include <iostream>
#include <limits.h>
#include <string>

#if defined(_WIN32)
  #include <winbase.h>
#else
  #include <unistd.h>
#endif

using namespace NETPLAY;

#define HELPER_LIBRARY_DIR  "resources"

// --- Helper functions --------------------------------------------------------

namespace NETPLAY
{
  /*!
   * \brief Get the path of the current process
   */
  std::string GetProcessPath(void)
  {
    char buff[PATH_MAX] = { };

#if defined(_WIN32)
    GetModuleFileName(NULL, buff, MAX_PATH);
#else
    readlink("/proc/self/exe", buff, sizeof(buff) - 1);
#endif

    return buff;
  }

  /*!
   * \brief Get the parent directory for a given file/folder
   */
  std::string GetParentDirectory(const std::string& strPath)
  {
    size_t pos = strPath.find_last_of("/\\");
    if (pos != std::string::npos)
      return strPath.substr(0, pos);

    return strPath;
  }

  /*!
   * \brief Get the directory for helper libraries
   */
  std::string GetHelperLibraryDir(void)
  {
    return GetParentDirectory(GetProcessPath()) + "/" + HELPER_LIBRARY_DIR;
  }
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
    std::cout << "Load game client:" << std::endl;
    std::cout << "  " << argv[0] << " --game <DLL> <system dir> <content dir> <save dir>" << std::endl;
    std::cout << "Load game client via proxy DLL:" << std::endl;
    std::cout << "  " << argv[0] << " --game <proxy DLL> <DLL> <system dir> <content dir> <save dir>" << std::endl;
    std::cout << "Load remote game client" << std::endl;
    std::cout << "  " << argv[0] << " --remote <address> <port>" << std::endl;
    std::cout << "Discover servers on the network:" << std::endl;
    std::cout << "  " << argv[0] << " --discover" << std::endl;
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

      GAME = new CDLLGame(CALLBACKS, props, GetHelperLibraryDir());
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

  if (!GAME || !GAME->Initialize())
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
