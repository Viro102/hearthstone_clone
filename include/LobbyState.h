#pragma once

#include <Common.h>

struct LobbyState {
    struct PlayerInfo {
        string name{};
        bool isReady{false};
    };

    vector<PlayerInfo> players;
};