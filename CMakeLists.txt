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
    src/interface/network/RequestHandler.cpp
    src/interface/network/RPCMethods.cpp
    src/interface/network/Server.cpp
    src/interface/network/SocketFactory.cpp
    src/keyboard/Keyboard.cpp
    src/keyboard/KeyboardAddon.cpp
    src/keyboard/KeyboardConsole.cpp
    src/keyboard/KeyboardMock.cpp
    src/log/Log.cpp
    src/log/LogAddon.cpp
    src/log/LogConsole.cpp
    src/utils/AbortableTask.cpp
    src/utils/Observer.cpp
    src/utils/PathUtils.cpp
    src/utils/ReadWriteLock.cpp
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
#  Add-on helper library macro
#
################################################################################

# Build, link and optionally package an add-on's additional library
macro (build_addon_library target addon_id prefix libs output_subdir)
  ADD_LIBRARY(${target} ${${prefix}_SOURCES})
  TARGET_LINK_LIBRARIES(${target} ${${libs}})
  set_target_properties(${target}
                        PROPERTIES
                        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${output_subdir}"
                        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${output_subdir}"
                        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${output_subdir}")
  if (NOT WIN32)
    set_target_properties(${target} PROPERTIES SUFFIX ".so")
  endif()

  IF(PACKAGE_ZIP OR PACKAGE_TGZ)
    IF(WIN32)
      # get the installation location for the addon's target
      get_property(dll_location TARGET ${target} PROPERTY LOCATION)
      # in case of a VC++ project the installation location contains a $(Configuration) VS variable
      # we replace it with ${CMAKE_BUILD_TYPE} (which doesn't cover the case when the build configuration
      # is changed within Visual Studio)
      string(REPLACE "$(Configuration)" "${CMAKE_BUILD_TYPE}" dll_location "${dll_location}")

      # install the generated DLL file
      INSTALL(PROGRAMS ${dll_location} DESTINATION ${addon_id}/${output_subdir}
              COMPONENT ${target})

      IF(CMAKE_BUILD_TYPE MATCHES Debug)
        # for debug builds also install the PDB file
        get_filename_component(dll_directory ${dll_location} DIRECTORY)
        INSTALL(FILES ${dll_directory}/${target}.pdb DESTINATION ${addon_id}/${output_subdir}
                COMPONENT ${target})
      ENDIF()
    ELSE(WIN32)
      INSTALL(TARGETS ${target} DESTINATION ${addon_id}/${output_subdir}
              COMPONENT ${target})
    ENDIF(WIN32)
  ELSE(PACKAGE_ZIP OR PACKAGE_TGZ)
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
      if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT OR NOT CMAKE_INSTALL_PREFIX)
        message(STATUS "setting install paths to match ${APP_NAME}: CMAKE_INSTALL_PREFIX: ${${APP_NAME_UC}_PREFIX}")
        set(CMAKE_INSTALL_PREFIX "${${APP_NAME_UC}_PREFIX}" CACHE PATH "${APP_NAME} install prefix" FORCE)
        set(CMAKE_INSTALL_LIBDIR "${${APP_NAME_UC}_LIB_DIR}" CACHE PATH "${APP_NAME} install libdir" FORCE)
      elseif(NOT CMAKE_INSTALL_PREFIX STREQUAL "${${APP_NAME_UC}_PREFIX}" AND NOT OVERRIDE_PATHS)
        message(FATAL_ERROR "CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} differs from ${APP_NAME} prefix ${${APP_NAME_UC}_PREFIX}. Please pass -DOVERRIDE_PATHS=1 to skip this check")
      else()
        if(NOT CMAKE_INSTALL_LIBDIR)
          set(CMAKE_INSTALL_LIBDIR "${CMAKE_INSTALL_PREFIX}/lib/${APP_NAME_LC}")
        endif()
      endif()
    else()
      set(CMAKE_INSTALL_LIBDIR "lib/${APP_NAME_LC}")
    endif()

    INSTALL(TARGETS  ${target}
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/addons/${addon_id}/${output_subdir})
  ENDIF(PACKAGE_ZIP OR PACKAGE_TGZ)
endmacro()

################################################################################
#
#  Guess the helper library suffix used in libXBMC_addon.h
#
################################################################################

if("${CORE_SYSTEM_NAME}" STREQUAL "windows")
    # pass
elseif("${CORE_SYSTEM_NAME}" STREQUAL "android")
    set(ADDON_HELPER_ARCH arm)
elseif("${CORE_SYSTEM_NAME}" STREQUAL "freebsd")
    if ("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
        set(ADDON_HELPER_ARCH x86_64-linux)
    else()
        set(ADDON_HELPER_ARCH i486-linux)
    endif()
elseif("${CORE_SYSTEM_NAME}" STREQUAL "ios")
    set(ADDON_HELPER_ARCH arm-osx)
elseif("${CORE_SYSTEM_NAME}" STREQUAL "linux")
    if ("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
        set(ADDON_HELPER_ARCH x86_64-linux)
    else()
        set(ADDON_HELPER_ARCH i486-linux)
    endif()
elseif("${CORE_SYSTEM_NAME}" STREQUAL "darwin" OR "${CORE_SYSTEM_NAME}" STREQUAL "osx")
    set(ADDON_HELPER_ARCH x86-osx)
elseif("${CORE_SYSTEM_NAME}" STREQUAL "rbpi")
    set(ADDON_HELPER_ARCH arm)
endif()

if(ADDON_HELPER_ARCH)
    set(ADDON_HELPER_SUFFIX "-${ADDON_HELPER_ARCH}")
endif()

################################################################################
#
#  Add-on helper libraries
#
################################################################################

set(XBMC_ADDON_HELPER XBMC_addon${ADDON_HELPER_SUFFIX})
set(KODI_GAME_HELPER KODI_game${ADDON_HELPER_SUFFIX})

build_addon_library(${XBMC_ADDON_HELPER}
                    ${PROJECT_NAME}
                    ADDON_HELPER_LIB
                    ADDON_HELPER_DEPLIBS
                    resources/library.xbmc.addon
)

build_addon_library(${KODI_GAME_HELPER}
                    ${PROJECT_NAME}
                    GAME_HELPER_LIB
                    GAME_HELPER_DEPLIBS
                    resources/library.kodi.game
)

include(CPack)

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

