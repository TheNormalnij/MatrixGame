// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CServerCore.h"
#include "net/transports/CServerTCPTransport.h"

CServerCore::CServerCore() {
    m_mainLoop = CServerMainLoop();
    m_pNetHandler = new CServerTCPTransport(m_mainLoop.GetLoop());
}

CServerCore::~CServerCore() {
    m_pNetHandler->Release();
}

void CServerCore::StartServer(std::string_view host, uint16_t port) {
    m_pNetHandler->Listen(host, port);

    m_pNetHandler->SetPacketHandler([](void* source, size_t len, char* data) {

    });

    m_mainLoop.Start();
}

void CServerCore::StopServer() {
    m_mainLoop.Stop();
    m_pNetHandler->Close();
}
