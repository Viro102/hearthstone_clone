cmake_minimum_required(VERSION 3.18)
project(Hearthstone_clone)

set(CMAKE_CXX_STANDARD 20)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SERVER_SOURCES
        src/networking/ServerMain.cpp
        src/networking/Server.cpp
        src/networking/Client.cpp)

add_executable(Server ${SERVER_SOURCES})

set(CLIENT_SOURCES
        src/Main.cpp
        src/game/Game.cpp
        src/game/Player.cpp
        src/game/Card.cpp
        src/game/Deck.cpp
        src/gui/GameScreen.cpp
        src/game/Slot.cpp
        src/networking/Client.cpp
        src/Mouse.cpp
        src/utility/Common.cpp)

add_executable(Client ${CLIENT_SOURCES})

target_include_directories(Server PRIVATE include)
target_include_directories(Client PRIVATE include)

# Dependencies
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
if (NOT json_FOUND)
    include(FetchContent)
    FetchContent_Declare(
            json
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/nlohmann/json/releases/download/v${JSON_VERSION}/json.tar.xz)
    FetchContent_MakeAvailable(json)
endif ()

target_link_libraries(Client nlohmann_json::nlohmann_json)
target_link_libraries(Server nlohmann_json::nlohmann_json pthread)
target_link_libraries(Client raylib)
