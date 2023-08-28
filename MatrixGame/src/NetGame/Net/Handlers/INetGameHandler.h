// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <vector>
#include <string_view>
#include <cstdint>

#include "shared/game/Game.h"
#include <shared/game/ICommandFactory.h>

class INetGameHandler {
public:
    virtual void OnGetGameInfo(std::string_view mapName, uint32_t seed, unsigned char side, unsigned char sideAiStatus) = 0;
    virtual void OnChangeGameState(EGameStatus status) = 0;
    virtual void OnGetTickCommands(size_t tick, std::vector<IGameCommand *> &commands) = 0;

    virtual ICommandFactory *GetCommandFactory() const noexcept = 0;
};
