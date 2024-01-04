#pragma once

#include <string>
#include <iostream>
#include <array>
#include <vector>
#include <memory>
#include <algorithm>
#include "raylib.h"

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
