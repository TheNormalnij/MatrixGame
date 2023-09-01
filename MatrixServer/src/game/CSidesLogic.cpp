// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CSidesLogic.h"

void CSidesLogic::SetAiEnabled(bool state) {
    for (int i = 0; i < MAX_SIDES_COUNT; i++) {
        m_sides[i].SetAiEnabled(state);
    }
}

CGameSide *CSidesLogic::GetFreeSide() noexcept {
    for (int i = 0; i < MAX_SIDES_COUNT; i++) {
        if (!m_sides[i].GetPlayer() && !m_sides[i].IsLocked()) {
            return &m_sides[i];
        }
    }
    return nullptr;
}

EGameSide CSidesLogic::FindPlayerSide(IPlayer *player) noexcept {
    for (int i = 0; i < MAX_SIDES_COUNT; i++) {
        if (m_sides[i].GetPlayer() == player) {
            return (EGameSide)i;
        }
    }
    return EGameSide::NONE;
}
