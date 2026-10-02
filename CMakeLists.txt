cmake_minimum_required(VERSION 3.18)
project(Hearthstone_clone)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(GAME_SOURCES
        src/game/Card.cpp
        src/game/Game.cpp
        src/game/Player.cpp
        src/game/Deck.cpp
        src/networking/Client.cpp
        src/networking/Net.cpp)

# The server uses POSIX sockets and signals directly and is only built on Linux/Unix.
# The client builds on Linux, macOS and Windows (MSVC or MinGW).
if (NOT WIN32)
    set(SERVER_SOURCES
            src/networking/ServerMain.cpp
            src/networking/Server.cpp
            ${GAME_SOURCES})

    add_executable(Server ${SERVER_SOURCES})
    set(EXECUTABLE_TARGETS Server)
endif ()

set(CLIENT_SOURCES
        src/Main.cpp
        src/gui/GameScreen.cpp
        src/gui/Button.cpp
        ${GAME_SOURCES})

add_executable(Client ${CLIENT_SOURCES})
list(APPEND EXECUTABLE_TARGETS Client)

# Default assets location baked into the binaries. When deploying elsewhere, set this at configure
# time, set HEARTHSTONE_ASSETS at run time, or run from a directory with assets/ or ../assets/.
set(HEARTHSTONE_ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets" CACHE PATH "Directory containing the game assets")

foreach (target ${EXECUTABLE_TARGETS})
    target_include_directories(${target} PRIVATE include)
    target_compile_definitions(${target} PRIVATE ASSETS_DIR="${HEARTHSTONE_ASSETS_DIR}")
    if (MSVC)
        target_compile_options(${target} PRIVATE /W3 /permissive-)
        # getenv() is fine here
        target_compile_definitions(${target} PRIVATE _CRT_SECURE_NO_WARNINGS)
    else ()
        target_compile_options(${target} PRIVATE -Wall -Wextra)
    endif ()
endforeach ()

# Dependencies
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(RAYLIB_VERSION 5.0)
find_package(raylib ${RAYLIB_VERSION} QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
            raylib
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz)
    FetchContent_MakeAvailable(raylib)
endif ()

set(JSON_VERSION 3.11.3)
find_package(nlohmann_json ${JSON_VERSION} QUIET)
if (NOT nlohmann_json_FOUND)
    include(FetchContent)
    FetchContent_Declare(
            json
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/nlohmann/json/releases/download/v${JSON_VERSION}/json.tar.xz)
    FetchContent_MakeAvailable(json)
endif ()

# Only the client needs raylib; the server is headless
target_link_libraries(Client PRIVATE nlohmann_json::nlohmann_json raylib Threads::Threads)
if (WIN32)
    target_link_libraries(Client PRIVATE ws2_32)
else ()
    target_link_libraries(Server PRIVATE nlohmann_json::nlohmann_json Threads::Threads)
endif ()
