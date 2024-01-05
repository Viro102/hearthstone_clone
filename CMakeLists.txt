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
        src/gui/Panel.cpp
        src/game/Slot.cpp
        src/utility/CardContainer.cpp
        src/networking/Client.cpp)

add_executable(Client ${CLIENT_SOURCES})

target_include_directories(Server PRIVATE include)
target_include_directories(Client PRIVATE include)

# Dependencies
set(RAYLIB_VERSION 4.5.0)
find_package(raylib ${RAYLIB_VERSION} QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
            raylib
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED)
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(raylib)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
    endif ()
endif ()

target_link_libraries(Client raylib)
target_link_libraries(Server raylib)