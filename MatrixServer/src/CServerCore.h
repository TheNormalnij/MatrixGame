// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <string_view>
#include "CServerMainLoop.h"
#include "net/transports/IServerTransport.h"
#include "net/transports/ITransportHandler.h"
#include "net/sessions/CSessionStore.h"
#include "game/IGame.h"
#include "game/CGameNetwork.h"

// Application main class
class CServerCore {
public:
    CServerCore();
    ~CServerCore();

    void StartServer(std::string_view host, uint16_t port);
    void StopServer();

private:
    CGameNetwork *m_pGameNetApi;
    CSessionStore m_sessionStore;
    CServerMainLoop m_mainLoop;
    IServerTransport *m_pNetHandler;
    IGame *m_game;
    ITransportHandler *m_pTransportHandler;

};
