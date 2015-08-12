# Overview

This add-on provides mutliplayer support for Kodi's Game API over the network. Netplay support is completely transparent; it basically MITM's the Game API and packs C structs and DLL function calls over the network.

When netplay is enabled, Kodi silently loads the netplay DLL instead of the actual game client. This is identical to how the libretro wrapper ([game.libretro](https://github.com/kodi-game/game.libretro)) loads libretro cores. Indeed, typically the libretro core is loaded by the libretro wrapper which is loaded by the netplay DLL.

Netplay is transparent to the game client. When a remote Kodi frontend is connected, the functions and their C structs are serilaized, packed across the network, and then deserialized, appearing to the remote Kodi instance as if the game client was a local DLL.

# Design

## Intercepting DLL functions

When a game client is launched, Kodi loads netplay which loads the game client. Functions meant for the game client are intercepted by netplay, and then forwarded to the game client DLL unmodified (zero-copy).

## Intercepting DLL callbacks

Callbacks are a bit trickier. Kodi uses helper libraries to allow game clients to communicate with the frontend. Instead of passing these helpers to the game client, netplay forges fake helper libraries (see the `lib/` directory) and gives these to the game client.

The fake helpers allow netplay to MITM the callbacks. When no remote clients are connected, callbacks are forwarded to Kodi unmodified (zero-copy) via the *real* helper libraries.

## Adding a remote frontend

When a remote Kodi instance connects, it is registered as an additional frontend. When the game client invokes a callback, the callback is teed and sent to some or all frontends. For example, the callback for video frames might be sent to all frontends, but the callback to open a file is only sent to the local frontend.

Local frontends receive the callback's arguments unmodified (zero-copy). When a callback is destined for a remote frontend, the callback and its arguments are also serialized using protobufs and sent across the network. At the remote frontend, the data is deserialized and sent to Kodi via its helper libraries. To Kodi, it looks as if the game client is being run locally.

## Connecting to a remote game client

Netplay can also be run in standalone mode (without a local game client). DLL function calls are sent to the remote game client, executed, and the results passed back to the standalone netplay instance.

As a remote frontend, this standalone netplay instance can also accept callbacks from the remote game client (such as rendering a video frame). The callback is deserialized, passed to Kodi via the helper libraries, executed, and the results are sent back to the remote game client.

## Putting it all together

1. Netplay must first load a game client. This can happen by launching a game in Kodi. Netplay can also be launched headless from the command line, in which case it runs in server-only mode with no local frontend.
2. Next, remote netplay instances login and register themselves as remote frontends.
3. Frontends (both local and remote) can call the game client's functions and receive the results.
4. The game client can invoke callbacks, which are teed and sent to some or all frontends.

# Building game.netplay

## Upgrading protoc to 2.6

Ubuntu <= 14.10 utopic requires a newer version of protoc. Add the line

```
deb http://archive.ubuntu.com/ubuntu/ vivid main
```

to your sources (see https://help.ubuntu.com/stable/ubuntu-help/addremove-sources.html) and install the newer version of protoc:

```shell
sudo apt-get install protobuf-compiler=2.6.1-1 libprotobuf-dev
```

## Building out-of-tree (recommended)

### Linux

Create and enter a build directory

```shell
mkdir game.netplay
cd game.netplay
```

Generate a build environment with config for debugging

```shell
cmake -DADDONS_TO_BUILD=game.netplay \
      -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_INSTALL_PREFIX=$HOME/workspace/xbmc/addons \
      -DPACKAGE_ZIP=1 \
      $HOME/workspace/xbmc/project/cmake/addons
```

The add-on can then be built with `make`.

## Building stand-alone (development)

Stand-alone builds are closer to "normal" software builds. The build system looks for its dependencies, by default with `/usr` and `/usr/local` prefixes.

To provide these dependencies yourself in a local working directory (`$HOME/kodi`), build Kodi with an installation prefix

```shell
./configure --prefix=$HOME/kodi
make
make install
```

Clone kodi-platform and create a CMake build directory

```shell
git clone https://github.com/xbmc/kodi-platform.git
cd kodi-platform
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_INSTALL_PREFIX=$HOME/kodi \
      ..
make
make install
```

The platform library was split from kodi-platform. Do the same as above for the new platform library:

```
git clone https://github.com/Pulse-Eight/platform.git
...
```

With these dependencies in place, the add-on can be built. Use the CMake command for kodi-platform above, or use this command to copy add-ons to your Kodi source tree:

```shell
cmake -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_PREFIX_PATH=$HOME/kodi \
      -DCMAKE_INSTALL_PREFIX=$HOME/workspace/xbmc/addons \
      -DPACKAGE_ZIP=1 \
      ..
```

## Building in-tree (cross-compiling)

Kodi's build system will fetch the add-on from the GitHub URL and git hash specified in [game.netplay.txt](https://github.com/garbear/xbmc/blob/retroplayer-15alpha2/project/cmake/addons/addons/game.netplay/game.netplay.txt).

### Linux

Ensure that kodi has been built successfully. Then, from the root of the source tree, run

```shell
make install DESTDIR=$HOME/kodi
```

Build the add-on

```shell
make -C tools/depends/target/binary-addons PREFIX=$HOME/kodi ADDONS="game.netplay"
```

The compiled .so can be found at

```
$HOME/kodi/lib/kodi/addons/game.netplay/game.netplay.so
```

To rebuild the add-on or compile a different one, clean the build directory

```shell
make -C tools/depends/target/binary-addons clean
```

### Windows

First, download and install [CMake](http://www.cmake.org/download/).

To compile on windows, open a command prompt at `tools\buildsteps\win32` and run the script:

```
make-addons.bat install game.netplay
```

Alternatively, generate the `kodi-addons.sln` Visual Studio solution and project files.

```
tools\windows\prepare-binary-addons-dev.bat
```

The generated solution can be found at

```
project\cmake\addons\build\kodi-addons.sln
```

Add-ons can be built individually through their specific project, or all at once by building the solution.

No source code is downloaded at the CMake stage; when the project is built, the add-on's source will be downloaded and compiled.

### OSX

Per [README.osx](https://github.com/garbear/xbmc/blob/retroplayer-15alpha2/docs/README.osx), enter the `tools/depends` directory and make the add-on:

```shell
cd tools/depends
make -C target/binary-addons ADDONS="game.netplay"
```

To rebuild the add-on or compile a different one, clean the build directory

```shell
make -C target/binary-addons clean
```
