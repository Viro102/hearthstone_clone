cmake_minimum_required(VERSION 3.18)
project(Hearthstone_clone)

set(CMAKE_CXX_STANDARD 20)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

add_executable(${PROJECT_NAME} src/Main.cpp
        src/game/Game.cpp
        include/Game.h
        src/game/Player.cpp
        include/Player.h
        include/Common.h
        src/game/Hand.cpp
        include/Hand.h
        src/game/Card.cpp
        include/Card.h
        src/game/Deck.cpp
        include/Deck.h
        src/game/Board.cpp
        include/Board.h
        src/gui/Panel.cpp
        include/Panel.h
        src/game/Slot.cpp
        include/Slot.h)

target_link_libraries(${PROJECT_NAME} raylib)
