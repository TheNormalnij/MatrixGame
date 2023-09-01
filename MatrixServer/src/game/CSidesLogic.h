// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "CGameSide.h"

class CSidesLogic {
public:
    CSidesLogic() = default;
    ~CSidesLogic() = default;

    CGameSide &GetSide(EGameSide side) noexcept { return m_sides[(int)side]; };

    void SetAiEnabled(bool state);
    CGameSide *GetFreeSide() noexcept;

    EGameSide FindPlayerSide(IPlayer *player) noexcept;

private:
    CGameSide m_sides[MAX_SIDES_COUNT];
};
