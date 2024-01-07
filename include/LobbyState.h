#pragma once

#include <Common.h>

struct LobbyState {
    struct PlayerInfo {
        string name{};
        bool isReady{false};
    };

    vector<PlayerInfo> players;

    bool canStart() const {
        if (players.size() < 2) {
            return false;
        }
        for (const auto& player : players) {
            if (!player.isReady) return false;
        }
        return !players.empty();
    }
};
