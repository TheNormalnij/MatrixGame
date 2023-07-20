// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "jobs/IServerJob.h"
#include <vector>
#include "uv.h"

// Application event loop controller
class CServerMainLoop {
public:
    CServerMainLoop() = default;
    ~CServerMainLoop() = default;

    void AddServerJob(IServerJob *job) { m_jobs.push_back(job); };

    void Start();
    void Stop();

    void DoTick();

    uv_loop_t *GetLoop();

private:
    std::vector<IServerJob *> m_jobs;
    uv_idle_t m_uvIdleHandler;
};
