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

IPlayer *CServerMatrixGame::GetSessionPlayer(ISession *session) const noexcept {
    return (IPlayer *)session->GetCustomData();
}

bool CServerMatrixGame::IsAllPlayersReady() const noexcept {
    for (const IPlayer* player : m_playersStore.GetPlayers()) {
        if (!player->IsReady()) {
            return false;
        }
    }
    return true;
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
    // TODO should send error codes back
    if (session->GetCustomData())
        return;

    if (m_currentStatus != EGameStatus::WAIT_PLAYERS) {
        return;
    }

    const size_t playersCount = m_playersStore.GetCount();

    if (playersCount + 1 > m_settings.maxPlayersCount) {
        return;
    }

    IPlayer *player = new CNetPlayer(session);
    session->SetCustomData(player);

    m_playersStore.AddPlayer(player);

    m_net.SendConnect(session);

    if (m_playersStore.GetCount() >= m_settings.maxPlayersCount) {
        m_currentStatus == EGameStatus::WAIT_PLAYERS_READY;
    }
}

void CServerMatrixGame::OnRequestSessionQuit(ISession *session) {
    IPlayer *player = (IPlayer*)session->GetCustomData();
    
    if (!player) {
        return;
    }

    if (m_currentStatus == EGameStatus::WAIT_PLAYERS) {
        // TODO  fix more cases here
        m_playersStore.RemovePlayer(player);

        delete player;
    }
}

void CServerMatrixGame::OnAskGameInfo(ISession *source) {
    m_net.SendGameInfo(source, m_settings.mapName);
}

void CServerMatrixGame::OnSessionReady(ISession *session) {
    IPlayer *player = GetSessionPlayer(session);
    if (!player) {
        return;
    }

    player->SetReady(true);

    if (m_currentStatus == EGameStatus::WAIT_PLAYERS_READY && IsAllPlayersReady()) {
        GameStart();
    }
}
