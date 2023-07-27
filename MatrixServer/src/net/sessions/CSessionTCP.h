// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "uv.h"
#include "CSessionBase.h"
#include "../transports/ITransportHandler.h"
#include "../transports/IServerTransport.h"

class CSessionTCP : public CSessionBase {
public:
    CSessionTCP(ITransportHandler* transportHandler, IServerTransport* transport);
    ~CSessionTCP();

    uv_tcp_t *GetHandler() { return &m_handler; };
    ITransportHandler *GetTransportHandler() const { return m_transportHandler; };

    void RegisterInEventLoop(uv_loop_t* loop);

    // Free itself after operation
    void CloseAndDestroy();

    void SendData(CRequest *req) override;

private:
    uv_tcp_t m_handler;
    ITransportHandler *m_transportHandler;
    IServerTransport *m_transport;
};