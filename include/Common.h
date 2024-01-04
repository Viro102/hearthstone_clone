#pragma once

#include <string>
#include <iostream>
#include <array>
#include <vector>
#include "raylib.h"
#include <memory>

using std::string;
using std::cout;
using std::array;
using std::vector;
using std::endl;

enum class GameState {
    MENU,
    LOBBY,
    GAMEPLAY,
    END
};
