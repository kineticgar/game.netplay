project(game.netpaly)
cmake_minimum_required(VERSION 2.8)

enable_language(CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(kodi REQUIRED)
find_package(kodiplatform REQUIRED)
find_package(platform REQUIRED)
include(FindProtobuf)
find_package(Protobuf REQUIRED)

file(GLOB ProtoFiles "${CMAKE_CURRENT_SOURCE_DIR}/messages/*.proto")
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${ProtoFiles})

include_directories(${INCLUDES}
                    ${kodi_INCLUDE_DIR}
                    ${kodiplatform_INCLUDE_DIRS}
                    ${platform_INCLUDE_DIRS}
                    ${CMAKE_CURRENT_BINARY_DIR} # For protobuf
                    ${PROJECT_SOURCE_DIR}/src)

list(APPEND DEPLIBS ${kodiplatform_LIBRARIES}
                    ${platform_LIBRARIES})

set(NETPLAY_SOURCES ${PROTO_SRCS}
                    src/client.cpp
                    src/interface/dll/DLLFrontend.cpp
                    src/interface/dll/DLLGame.cpp
                    src/interface/network/NetworkFrontend.cpp
                    src/interface/network/NetworkGame.cpp
                    src/log/Log.cpp
                    src/log/LogAddon.cpp
                    src/log/LogConsole.cpp
                    src/Netplay.cpp)

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

build_addon(game.netplay NETPLAY DEPLIBS)

include(CPack)
