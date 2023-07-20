// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <string_view>
#include "uv.h"

class CServerMainLoop {
public:
    CServerMainLoop();
    ~CServerMainLoop();

    void Start();
    void Stop();

    void DoTick();

    uv_loop_t *GetLoop();
};
