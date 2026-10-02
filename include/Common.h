#pragma once

#include <string>
#include <iostream>
#include <array>
#include <vector>
#include <memory>
#include <algorithm>
#include <utility>
#include <nlohmann/json.hpp>
#include <functional>
#include <optional>
#include <stdexcept>
#include <cstdlib>
#include <filesystem>


using std::string;
using std::cout;
using std::array;
using std::vector;
using std::endl;
using nlohmann::json;

// Default assets directory, set by CMake (HEARTHSTONE_ASSETS_DIR) to the source tree's assets folder.
#ifndef ASSETS_DIR
#define ASSETS_DIR "../assets"
#endif

// Path of an asset file. Looks in $HEARTHSTONE_ASSETS, then the configured ASSETS_DIR, then
// ../assets and assets relative to the working directory, so a binary copied to another machine
// still finds assets placed next to it.
inline string assetPath(const string &name) {
    if (const char *env = std::getenv("HEARTHSTONE_ASSETS"); env != nullptr && *env != '\0') {
        return string(env) + "/" + name;
    }
    for (const string &dir: {string(ASSETS_DIR), string("../assets"), string("assets")}) {
        std::error_code error;
        if (std::filesystem::exists(dir + "/" + name, error)) {
            return dir + "/" + name;
        }
    }
    return string(ASSETS_DIR) + "/" + name;
}
