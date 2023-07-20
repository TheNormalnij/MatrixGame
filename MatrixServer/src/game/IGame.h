// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "players/IPlayer.h"
#include "IGameCommand.h"
#include "../jobs/IServerJob.h"

class IGame : public IServerJob {
public:
    virtual void HandleCommand(IPlayer source, IGameCommand command) = 0;
    virtual void OnRequestPlayerJoin(IPlayer source) = 0;
    virtual void OnRequestPlayerQuit(IPlayer source) = 0;

    virtual void OnPlayerReady(IPlayer source) = 0;
};