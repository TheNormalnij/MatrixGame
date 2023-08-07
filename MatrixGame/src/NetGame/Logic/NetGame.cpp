// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "NetGame.h"
#include "NetGameForm.h"
#include "../Net/Clients/ClientTCP.h"

CNetGame::CNetGame(HINSTANCE hInstance, SMatrixSettings *set) {
    m_hAppInstance = hInstance;
    m_pMatrixSettings = set;
    m_pClient = new CClientTCP();
    m_pServerApi = new CServerAPI(m_pClient);
}

bool CNetGame::StartNetworkGame(std::string_view host) {
    ConnectGame(host);

    const bool connectSuccess = ConnectGame(host);
    if (!connectSuccess) {
        return false;
    }

    //m_serverApi.

    StartGame((wchar_t*)L"Shit", 0);

    return true;
}

bool CNetGame::ConnectGame(std::string_view host) {
    m_pServerConnection = new CServerConnection(host, m_pClient, m_pServerApi);
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

void CNetGame::StartGame(wchar_t *map, uint32_t seed) {
    SMatrixTextParams textReplace = {0};
    m_currentGame.Init(m_hAppInstance, NULL, map, seed, m_pMatrixSettings, &textReplace);

    CNetGameForm formgame;

    m_currentGame.RunGameLoop(&formgame);

    //m_currentGame.SaveResult(rgs);
    m_currentGame.SafeFree();

}