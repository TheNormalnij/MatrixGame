// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "NetGame.h"
#include "NetGameForm.h"
#include "Clients/CClientTCP.h"
#include <future>

bool CNetGame::StartNetworkGame(std::string_view host) {
    const bool connectSuccess = ConnectGame(host);
    if (!connectSuccess) {
        return false;
    }

   

    return true;
}

bool CNetGame::ConnectGame(std::string_view host) {
    static std::promise<bool> *waitingConnection = nullptr;
    if (waitingConnection) {
        return false;
    }

    m_pClient->Connect(host, [](INetworkClient *client, bool success, std::string_view error) {
        if (waitingConnection) {
            waitingConnection->set_value(success);
        }
    });

    waitingConnection->get_future().wait();

    const bool success = waitingConnection;
    waitingConnection = nullptr;
    return success;
}

void CNetGame::StartNetworkGame(wchar *map, uint32_t seed) {
    SMatrixTextParams textReplace = {0};
    m_currentGame.Init(m_hAppInstance, NULL, map, seed, m_pMatrixSettings, &textReplace);

    CNetGameForm formgame;

    m_currentGame.RunGameLoop(&formgame);

    //m_currentGame.SaveResult(rgs);
    m_currentGame.SafeFree();

}

void CNetGame::StopNetworkGame() {

}
