// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

constexpr const int TICK_DURATION_MS = 100;  // 10 ticks per second

enum class EGameStatus {
    WAIT_PLAYERS,
    WAIT_PLAYERS_READY,
    STARTING,
    RUNNING,
    STOPPING,
    FINISHED,
};
