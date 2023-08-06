// MatrixServer - SR2 Planetary battles server 
// Copyright (C) 2023, Uladzislau TheNormalnij Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "IGame.h"
#include <shared/game/CCommandLog.h>
#include "CGameNetwork.h"
#include <list>
#include <shared/game/Game.h>

class CServerMatrixGame : public IGame {
public:
    CServerMatrixGame();
    ~CServerMatrixGame() = default;

    // IGame interface
    void DoTick() override;
    void HandleCommand(IGameCommand *command) override;

    void OnRequestSessionStart(ISession *source) override;
    void OnRequestSessionQuit(ISession *source) override;

    void OnPlayerReady(IPlayer *source) override;

private:
    void GameStart();
    void GameStop();

private:
    EGameStatus m_currentStatus;
    size_t m_currentTick;
    CCommandLog m_commandLog;
    CGameNetwork m_net;

    std::string_view m_mapName;
};