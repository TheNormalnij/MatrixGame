// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "MatrixGameDllEx.hpp"
#include "MatrixGame.h"
#include <string_view>
#include "../Net/Clients/IClient.h"
#include "../Net/ServerApi.h"
#include "../Net/ServerConnection.h"

class CNetGame {
public:
    CNetGame(HINSTANCE hInstance, SMatrixSettings *set);
    ~CNetGame() = default;

    bool StartNetworkGame(std::string_view host);

private:
    bool ConnectGame(std::string_view host);
    void StartGame(wchar_t *map, uint32_t seed);

private:
    HINSTANCE m_hAppInstance;
    SMatrixSettings *m_pMatrixSettings;
    CGame m_currentGame;
    CServerAPI *m_pServerApi;
    CServerConnection *m_pServerConnection;
    INetworkClient *m_pClient;
};