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

// Set by CMake to the absolute assets directory, so the binaries work from any working directory.
#ifndef ASSETS_DIR
#define ASSETS_DIR "../assets"
#endif

using std::string;
using std::cout;
using std::array;
using std::vector;
using std::endl;
using nlohmann::json;