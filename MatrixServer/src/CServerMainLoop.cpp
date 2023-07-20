// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CServerMainLoop.h"
#include "uv.h"

void CServerMainLoop::Start() {
    uv_loop_t *loop = uv_default_loop();
    loop->data = this;

    uv_idle_init(loop, &m_uvIdleHandler);

    m_uvIdleHandler.data = this;
    uv_idle_start(&m_uvIdleHandler, [](uv_idle_t *handle) {
        CServerMainLoop* self = (CServerMainLoop *)handle->data;
        self->DoTick();
    });

    uv_run(loop, UV_RUN_DEFAULT);
}

void CServerMainLoop::Stop() {
    uv_idle_stop(&m_uvIdleHandler);
    m_uvIdleHandler.data = nullptr;

    uv_loop_t *loop = uv_default_loop();
    uv_loop_close(loop);
}

void CServerMainLoop::DoTick() {
    for (IServerJob* job : m_jobs) {
        job->DoTick();
    }
}

uv_loop_t *CServerMainLoop::GetLoop() {
    return uv_default_loop();
}
