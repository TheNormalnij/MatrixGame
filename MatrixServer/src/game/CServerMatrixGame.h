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
#include "SMatrixGameSettings.h"
#include "CPlayersStore.h"

class CServerMatrixGame : public IGame {
public:
    CServerMatrixGame();
    ~CServerMatrixGame() = default;

    // IGame interface
    void DoTick() override;
    void HandleCommand(IGameCommand *command) override;

    void OnRequestSessionStart(ISession *source) override;
    void OnRequestSessionQuit(ISession *source) override;

    void OnAskGameInfo(ISession *source) override;

    void OnSessionReady(ISession *source) override;

private:
    void GameStart();
    void GameStop();

    IPlayer *GetSessionPlayer(ISession *session) const noexcept;
    bool IsAllPlayersReady() const noexcept;

private:
    EGameStatus m_currentStatus;
    size_t m_currentTick;
    CCommandLog m_commandLog;
    CGameNetwork m_net;
    SMatrixGameSettings m_settings;
    CPlayersStore m_playersStore;
};
