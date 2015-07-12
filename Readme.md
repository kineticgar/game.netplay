# game.netplay

Netplay support for the Kodi Game API

# Upgrading protoc to 2.6

Ubuntu <= 14.10 utopic requires a newer version of protoc. Add the line

```
deb http://archive.ubuntu.com/ubuntu/ vivid main
```

to your sources (see https://help.ubuntu.com/stable/ubuntu-help/addremove-sources.html) and install the newer version of protoc:

```shell
sudo apt-get install protobuf-compiler=2.6.1-1 libprotobuf-dev
```

