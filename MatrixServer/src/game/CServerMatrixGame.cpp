// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CServerMatrixGame.h"
#include "players/CNetPlayer.h"

CServerMatrixGame::CServerMatrixGame() {
    m_currentStatus = EGameStatus::WAIT_PLAYERS;
    m_currentTick = 0;
    m_net = CGameNetwork();
    m_mapName = "TRAINING.CMAP";
}

void CServerMatrixGame::GameStart(){
    if (m_currentStatus != EGameStatus::WAIT_PLAYERS_READY) {
        return;
    }

    m_currentStatus = EGameStatus::RUNNING;
    m_net.SendGameStatusChanged(m_currentStatus);
};

void CServerMatrixGame::GameStop(){
    if (m_currentStatus != EGameStatus::RUNNING) {
        return;
    }

    m_net.SendGameStatusChanged(m_currentStatus);
}

void CServerMatrixGame::DoTick() {
    // Skip if not running
    if (m_currentStatus != EGameStatus::RUNNING) {
        return;
    }

    // Increase tick counter
    m_currentTick++;

    // Send current tick commands
    m_net.SendTickCommands(m_currentTick, *m_commandLog.GetTickCommands(m_currentTick));
}

void CServerMatrixGame::HandleCommand(IGameCommand *command) {
    m_commandLog.PushCommand(m_currentTick + 1, command);
}

void CServerMatrixGame::OnRequestSessionStart(ISession *session) {
    if (session->GetCustomData())
        return;

    IPlayer *player = new CNetPlayer(session);
    session->SetCustomData(player);

    m_net.SendConnect(session);
}

void CServerMatrixGame::OnRequestSessionQuit(ISession *session) {
    IPlayer *player = (IPlayer*)session->GetCustomData();
    
    delete player;
}

void CServerMatrixGame::OnAskGameInfo(ISession *source) {
    m_net.SendGameInfo(source, m_mapName);
}

void CServerMatrixGame::OnPlayerReady(IPlayer *source) {
    GameStart();
}
