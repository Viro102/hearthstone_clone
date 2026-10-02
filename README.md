Semester project focused on networking and multithreading

## Building

Requires CMake 3.18+ and a C++20 compiler. raylib 5.0 and nlohmann_json 3.11.3 are used from the
system if installed, otherwise downloaded automatically.

### Linux (server and client)

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/Server   # listens on port 10322, stop with Ctrl+C
./build/Client
```

The raylib build needs the X11 development packages (on Debian/Ubuntu: `libx11-dev libxrandr-dev
libxinerama-dev libxcursor-dev libxi-dev libgl1-mesa-dev`).

### Windows (client only)

The server is Linux-only; the Windows client connects to a Linux server.

With Visual Studio 2019 16.10+ or 2022 (from a Developer Command Prompt):

```bat
cmake -S . -B build
cmake --build build --config Release
build\Release\Client.exe
```

With MinGW-w64 (the posix-threads variant, needed for `std::jthread`), e.g. from MSYS2:

```sh
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

To cross-compile from Linux, use a toolchain file that sets `CMAKE_SYSTEM_NAME Windows` and
`CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++-posix`.

### Assets

The binaries look for `assets/` in this order: the `HEARTHSTONE_ASSETS` environment variable, the
directory configured at build time (`-DHEARTHSTONE_ASSETS_DIR=...`, defaults to this repository's
`assets/`), then `../assets` and `assets` relative to the working directory. When copying a binary to
another machine, put the `assets` folder next to it.
