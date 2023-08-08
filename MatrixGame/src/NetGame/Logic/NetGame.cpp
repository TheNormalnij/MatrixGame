// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "NetGame.h"
#include "NetGameForm.h"
#include "../Net/Clients/ClientTCP.h"
#include "../Net/Handlers/CGameNetDataHandler.h"
#include "utils.hpp"

constexpr time_t SERVER_TIMEOUT_SEC = 30;

CNetGame::CNetGame(HINSTANCE hInstance, SMatrixSettings *set) {
    m_hAppInstance = hInstance;
    m_pMatrixSettings = set;
    m_pClient = new CClientTCP();
    m_pServerApi = new CServerAPI(m_pClient);
    m_pServerConnection = nullptr;
    m_pNetDataHandler = new CGameNetDataHandler(&m_serverSync);
}

bool CNetGame::StartNetworkGame(std::string_view host) {
    const bool connectSuccess = ConnectGame(host);
    if (!connectSuccess) {
        return false;
    }

    m_pServerApi->SendAskGameInfo();

    const bool isGameReady = WaitForGameInfoReady();

    if (isGameReady) {
        StartGame();
    }

    return isGameReady;
}

bool CNetGame::WaitForGameInfoReady() {
    const time_t startWaitSec = time(nullptr);
    while (true) {
        m_pClient->DoUpdate();
        if (m_serverSync.IsGameInfoReady()) {
            return true;
        }
        if (time(nullptr) - startWaitSec > SERVER_TIMEOUT_SEC) {
            return false;
        }
    }
}

bool CNetGame::ConnectGame(std::string_view host) {
    m_pServerConnection = new CServerConnection(host, m_pClient, m_pServerApi, m_pNetDataHandler);
    m_pServerConnection->Connect();

    while (true) {
        m_pClient->DoUpdate();

        if (m_pServerConnection->GetConnectStatus() == EServerConnectionStatus::CONNECTED) {
            return true;
        }
        else if (m_pServerConnection->GetConnectStatus() == EServerConnectionStatus::DISCONNECTED) {
            return false;
        }
    }
}

void CNetGame::StartGame() {
    std::wstring wstrMapName = utils::to_wstring(m_serverSync.GetMapName().data());
    const uint32_t seed = m_serverSync.GetGameSeed();

    SMatrixTextParams textReplace = {0};
    m_currentGame.Init(m_hAppInstance, NULL, wstrMapName.data(), seed, m_pMatrixSettings, &textReplace);

    CNetGameForm formgame;

    m_currentGame.RunGameLoop(&formgame);

    //m_currentGame.SaveResult(rgs);
    m_currentGame.SafeFree();

}
