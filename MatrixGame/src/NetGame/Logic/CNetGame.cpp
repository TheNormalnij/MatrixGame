// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CNetGame.h"
#include "CNetGameForm.h"
#include "../Net/Clients/ClientTCP.h"
#include "../Net/Handlers/CGameNetDataHandler.h"
#include "CNetGameLogic.h"
#include "CNetOrderProcessor.h"
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

    const bool isGameInfoReady = WaitForGameInfoReady();

    if (isGameInfoReady) {
        StartGame();
    }

    return isGameInfoReady;
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

    const time_t startWaitSec = time(nullptr);
    while (true) {
        m_pClient->DoUpdate();

        if (m_pServerConnection->GetConnectStatus() == EServerConnectionStatus::CONNECTED) {
            return true;
        }
        else if (m_pServerConnection->GetConnectStatus() == EServerConnectionStatus::DISCONNECTED) {
            return false;
        }

         if (time(nullptr) - startWaitSec > SERVER_TIMEOUT_SEC) {
            m_pServerConnection->Disconnect();
            return false;
        }
    }
}

void CNetGame::StartGame() {
    InitGame();
    
    ApplySides();

    Run();
}

void CNetGame::InitGame() {
    std::wstring wstrMapName = utils::to_wstring(m_serverSync.GetMapName().data());
    const uint32_t seed = m_serverSync.GetGameSeed();

    SMatrixTextParams textReplace = {0};
    textReplace.startText = L"Press accept to start";

    m_currentGame.Init(m_hAppInstance, NULL, wstrMapName.data(), seed, m_pMatrixSettings, &textReplace);

    SETFLAG(g_Flags, GFLAG_KEEPALIVE);

    g_MatrixMap->GetRandom().Reset(seed);

    g_MatrixMap->m_Difficulty.k_damage_enemy_to_player = 1.0;
    g_MatrixMap->m_Difficulty.k_friendly_fire = 1.0;
    g_MatrixMap->m_Difficulty.k_time_before_maintenance = 1.0;
}

void CNetGame::ApplySides() {
    int playerSide = m_serverSync.GetPlayerSide();

    // Ломает менюшку постройки
    g_MatrixMap->SetPlayerSide(playerSide);
    for (int i = 1; i < MAX_SIDES_COUNT; i++) {
        g_MatrixMap->GetSideById(i)->SetAiEnabled(m_serverSync.IsAiEnabled(i));
    }
}

void CNetGame::Run() {
    auto side = g_MatrixMap->GetSideById(m_serverSync.GetPlayerSide());

    CNetOrderProcessor orderProcessor(side, m_pServerApi);
    COrderController orderController(&orderProcessor, side);

    CNetGameLogic logic = CNetGameLogic(m_pClient, m_pServerApi, &m_serverSync, g_MatrixMap);
    CNetGameForm formgame = CNetGameForm(&logic, &orderController);

    m_currentGame.RunGameLoop(&formgame);

    m_currentGame.SafeFree();
}
