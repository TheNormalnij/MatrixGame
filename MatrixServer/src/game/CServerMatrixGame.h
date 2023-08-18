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
#include <shared/game/CCommandFactory.h>
#include "SMatrixGameSettings.h"
#include "CPlayersStore.h"

class CServerMatrixGame : public IGame {
public:
    CServerMatrixGame(CGameNetwork *netApi);
    ~CServerMatrixGame() = default;

    // Inherited via IGame
    void Update() override;
    void HandleCommand(IGameCommand *command) override;

    void OnRequestSessionStart(ISession *source) override;
    void OnRequestSessionQuit(ISession *source) override;

    void OnAskGameInfo(ISession *source) override;

    void OnSessionReady(ISession *source) override;

    ICommandFactory *GetCommandFactory() const noexcept override { return (ICommandFactory*)&m_commandFactory; };

private:
    void GameStart();
    void GameStop();

    void DoTick();

    IPlayer *GetSessionPlayer(ISession *session) const noexcept;
    bool IsAllPlayersReady() const noexcept;

    // Update status and sync with clients
    void UpdateStatus(EGameStatus newStatus);

private:
    EGameStatus m_currentStatus;
    size_t m_currentTick;
    uint64_t m_startTime;
    uint64_t m_lastTickTime;
    CCommandLog m_commandLog;
    CCommandFactory m_commandFactory;
    CGameNetwork *m_pNet;
    SMatrixGameSettings m_settings;
    CPlayersStore m_playersStore;
};
