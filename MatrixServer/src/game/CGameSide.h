// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <shared/game/Game.h>
#include "players/IPlayer.h"

class CGameSide {
public:
    CGameSide() : m_locked(false), m_ai(false), m_player(nullptr){};
    ~CGameSide() = default;

    void SetPlayer(IPlayer *player) { m_player = player; };
    IPlayer *GetPlayer() const noexcept { return m_player; };

    // Будет ли работать аи на этой стороне
    void SetAiEnabled(bool state) { m_ai = state; };
    bool IsAiEnabled() const noexcept { return m_ai; };

    // Можно ли подключатсья к стороне
    bool IsLocked() const noexcept { return m_locked; };
    void SetLocked(bool state) { m_locked = state; };

private:
    // TODO flags via std::bitset
    bool m_locked;
    bool m_ai;
    IPlayer *m_player;
};
