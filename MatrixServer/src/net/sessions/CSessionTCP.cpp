// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CSessionTCP.h"

CSessionTCP::CSessionTCP(ITransportHandler *transportHandler, IServerTransport* transport) {
    m_handler.data = this;
    m_transportHandler = transportHandler;
    m_transport = transport;
}

CSessionTCP::~CSessionTCP() {
    m_handler.data = nullptr;
}

void CSessionTCP::RegisterInEventLoop(uv_loop_t *loop) {
    uv_tcp_init(loop, &m_handler);
}

void CSessionTCP::CloseAndDestroy() {
    uv_close((uv_handle_t *)&m_handler, [](uv_handle_t * handle) {
        delete (CSessionTCP *)handle->data;
    });
}

void CSessionTCP::SendData(CRequest* req) {
    m_transport->SendData(this, req);
}
