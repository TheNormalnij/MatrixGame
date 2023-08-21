// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CServerMatrixGame.h"
#include "players/CNetPlayer.h"
#include <ctime>

CServerMatrixGame::CServerMatrixGame(CGameNetwork *netApi) {
    m_currentStatus = EGameStatus::WAIT_PLAYERS;
    m_currentTick = 0;
    m_lastTickTime = 0;
    m_startTime = 0;
    m_pNet = netApi;
}

void CServerMatrixGame::GameStart(){
    if (m_currentStatus != EGameStatus::WAIT_PLAYERS) {
        return;
    }

    m_startTime = std::clock();

    UpdateStatus(EGameStatus::RUNNING);
};

void CServerMatrixGame::GameStop(){
    if (m_currentStatus != EGameStatus::RUNNING) {
        return;
    }

    m_pNet->SendGameStatusChanged(m_currentStatus);
}

void CServerMatrixGame::DoTick() {
    // Increase tick counter
    m_currentTick++;

    // Send current tick commands
    auto commands = m_commandLog.GetTickCommands(m_currentTick);

    m_pNet->SendTickCommands(m_currentTick, commands);
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

void CServerMatrixGame::UpdateStatus(EGameStatus newStatus) {
    m_currentStatus = newStatus;

    m_pNet->SendGameStatusChanged(newStatus);
}

void CServerMatrixGame::Update() {
    // Skip if not running
    if (m_currentStatus != EGameStatus::RUNNING) {
        return;
    }

    const uint64_t currentTime = std::clock();
    if (m_startTime + TICK_DURATION_MS * CLOCKS_PER_SEC / 1000.0 * (m_currentTick + 1) < currentTime) {
        DoTick();
    }
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

    m_pNet->SendConnect(session);

    if (m_playersStore.GetCount() >= m_settings.maxPlayersCount) {
        UpdateStatus(EGameStatus::WAIT_PLAYERS_READY);
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
    m_pNet->SendGameInfo(source, m_settings.mapName);
}

void CServerMatrixGame::OnSessionReady(ISession *session) {
    IPlayer *player = GetSessionPlayer(session);
    if (!player) {
        return;
    }

    player->SetReady(true);

    if (m_currentStatus == EGameStatus::WAIT_PLAYERS && IsAllPlayersReady()) {
        GameStart();
    }
}
