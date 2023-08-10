// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "players/IPlayer.h"
#include <shared/game/IGameCommand.h>
#include "../jobs/IServerJob.h"

class IGame : public IServerJob {
public:
    virtual ~IGame() = default;
    virtual void HandleCommand(IGameCommand *command) = 0;

    virtual void OnRequestSessionStart(ISession *source) = 0;
    virtual void OnRequestSessionQuit(ISession *source) = 0;

    virtual void OnSessionReady(ISession *source) = 0;
    virtual void OnAskGameInfo(ISession *source) = 0;
};
