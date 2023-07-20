// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <string_view>
#include "CServerMainLoop.h"
#include "net/transports/IServerTransport.h"

class CServerCore {
public:
    CServerCore();
    ~CServerCore();

    void StartServer(std::string_view host, uint16_t port);
    void StopServer();

private:
    CServerMainLoop m_mainLoop;
    IServerTransport *m_pNetHandler;
};
