// MatrixServer - SR2 Planetary battles server 
// Copyright (C) 2023, Uladzislau TheNormalnij Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "IGame.h"
#include "CCommandLog.h"
#include <list>

enum class EGameStatus {
    WAIT_PLAYERS,
    WAIT_PLAYERS_READY,
    STARTING,
    RUNNING,
    STOPPING,
    FINISHED,
};

class CServerMatrixGame : public IGame {
public:
    CServerMatrixGame();
    ~CServerMatrixGame() = default;

    // IGame interface
    void DoTick() override;
    void HandleCommand(IGameCommand command) override;
    void OnRequestPlayerJoin(IPlayer player) override;
    void OnRequestPlayerQuit(IPlayer player) override;
    void OnPlayerReady(IPlayer source) override;

private:
    void GameStart();
    void GameStop();

    void SendCurrentTickCommands();

private:
    EGameStatus m_currentStatus;
    size_t m_currentTick;
    CCommandLog m_commandLog;
};
