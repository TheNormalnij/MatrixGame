// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <list>
#include "players/IPlayer.h"

class CPlayersStore {
public:
    CPlayersStore() = default;
    ~CPlayersStore() = default;

    auto &GetPlayers() const noexcept { return m_players; };
    void AddPlayer(IPlayer *player);
    void RemovePlayer(IPlayer *player);
    bool HasPlayer(IPlayer *player) const noexcept;
    size_t GetCount() const noexcept { return m_players.size(); };

private:
    std::list<IPlayer*> m_players;
};