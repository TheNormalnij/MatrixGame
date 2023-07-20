// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CServerMainLoop.h"
#include "uv.h"

CServerMainLoop::CServerMainLoop() {}

CServerMainLoop::~CServerMainLoop() {}


void CServerMainLoop::Start() {
    uv_loop_t *loop = uv_default_loop();
    loop->data = this;

    //uv_timer_init(loop)
    uv_run(loop, UV_RUN_DEFAULT);
}

void CServerMainLoop::Stop() {
    uv_loop_t *loop = uv_default_loop();
    uv_loop_close(loop);
}

void CServerMainLoop::DoTick() {

}

uv_loop_t *CServerMainLoop::GetLoop() {
    return uv_default_loop();
}
