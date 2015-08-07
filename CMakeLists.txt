project(game.netplay)
cmake_minimum_required(VERSION 2.8)

enable_language(CXX)

################################################################################
#
#  CMake modules
#
################################################################################

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(kodi REQUIRED)
find_package(kodiplatform REQUIRED)
find_package(platform REQUIRED)
include(FindProtobuf)
find_package(Protobuf REQUIRED)

################################################################################
#
#  Generate messages
#
################################################################################

file(GLOB ProtoFiles "${CMAKE_CURRENT_SOURCE_DIR}/messages/*.proto")

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${ProtoFiles})

################################################################################
#
#  Add sources, headers and libraries
#
################################################################################

include_directories(
    ${INCLUDES}
    ${kodi_INCLUDE_DIR}
    ${kodiplatform_INCLUDE_DIRS}
    ${platform_INCLUDE_DIRS}
    ${PROTOBUF_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR} # For protobuf
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src
)

set(NETPLAY_SOURCES
    ${PROTO_SRCS}
    src/filesystem/StatStructure.cpp
    src/interface/dll/DLLFrontend.cpp
    src/interface/dll/DLLGame.cpp
    src/interface/dll/FrontendCallbackLib.cpp
    src/interface/dll/FrontendCallbacks.cpp
    src/interface/FrontendManager.cpp
    src/interface/network/Client.cpp
    src/interface/network/FrontendHandler.cpp
    src/interface/network/GameHandler.cpp
    src/interface/network/NetworkFrontend.cpp
    src/interface/network/NetworkGame.cpp
    src/interface/network/platform/PlatformSocket.cpp
    src/interface/network/Server.cpp
    src/interface/network/SocketFactory.cpp
    src/log/Log.cpp
    src/log/LogAddon.cpp
    src/log/LogConsole.cpp
    src/utils/AbortableTask.cpp
    src/utils/Observer.cpp
    src/utils/PathUtils.cpp
    src/utils/SignalHandler.cpp
    src/utils/StringUtils.cpp
    src/utils/Version.cpp
)

if(WIN32)
    # TODO
else(WIN32)
    list(APPEND NETPLAY_SOURCES
        src/filesystem/linux/LinuxPoller.cpp
        src/interface/network/linux/LinuxServerSocket.cpp
        src/interface/network/linux/LinuxSocket.cpp
    )
endif(WIN32)

set(STANDALONE_SOURCES
    ${NETPLAY_SOURCES}
    src/main.cpp
)

set(ADDON_SOURCES
    ${NETPLAY_SOURCES}
    src/client.cpp
)

set(ADDON_HELPER_LIB_SOURCES
    lib/library.xbmc.addon/libXBMC_addon.cpp
)

set(GAME_HELPER_LIB_SOURCES
    lib/library.kodi.game/libKODI_game.cpp
)

list(APPEND DEPLIBS
    ${kodiplatform_LIBRARIES}
    ${platform_LIBRARIES}
    ${PROTOBUF_LIBRARIES}
)

set(STANDALONE_LIBS
    ${DEPLIBS}
    dl
)

################################################################################
#
#  Warnings
#
################################################################################

# Uncomment to compile with pedantic warnings. Disabled because of excessive
# warnings despite valid behavior in add-on API callbacks:
#
#   warning: ISO C++ forbids casting between pointer-to-function and pointer-to-object
#
#set(WARN_PEDANTIC "-Wpedantic")

if(MSVC)
    # Force to always compile with W4
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    # Update if necessary
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long ${WARN_PEDANTIC}")
endif()

################################################################################
#
#  Standalone target
#
################################################################################

add_executable(netplay_server ${STANDALONE_SOURCES})

target_link_libraries(netplay_server ${STANDALONE_LIBS})

################################################################################
#
#  Add-on target
#
################################################################################

build_addon(${PROJECT_NAME} ADDON DEPLIBS)

################################################################################
#
#  Helper libraries (TODO: Fix platform suffix for helper libraries)
#
################################################################################

set(ADDON_HELPER_ARCH x86_64-linux) # TODO

set(XBMC_ADDON_HELPER XBMC_addon-${ADDON_HELPER_ARCH})
set(KODI_GAME_HELPER KODI_game-${ADDON_HELPER_ARCH})

build_addon_library(${XBMC_ADDON_HELPER} ${PROJECT_NAME} ADDON_HELPER_LIB ADDON_HELPER_DEPLIBS)
build_addon_library(${KODI_GAME_HELPER} ${PROJECT_NAME} GAME_HELPER_LIB GAME_HELPER_DEPLIBS)

include(CPack)

