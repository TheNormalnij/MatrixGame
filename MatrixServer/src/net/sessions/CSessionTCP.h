// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "uv.h"
#include "ISession.h"

class CSessionTCP : public ISession {
public:
    CSessionTCP();
    ~CSessionTCP();

    uv_tcp_t *GetHandler() { return &m_handler; };
    void RegisterInEventLoop(uv_loop_t* loop);

    // Free itself after operation
    void CloseAndDestroy();

private:
    uv_tcp_t m_handler;
};