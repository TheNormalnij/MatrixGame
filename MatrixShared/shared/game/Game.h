// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

 // 20 ticks per second
constexpr const int TICK_DURATION_MS = 100;

constexpr const int TICK_STEP = 25;

constexpr const int ROBOT_MAX_WEAPONS = 5;

constexpr const int MAX_SIDES_COUNT = 5;

enum class EGameSide {
    NONE = -1,

    NEUTRAL,
    YELLOW,
    RED,
    BLUE,
    GREEN,
};

enum class EGameStatus {
    WAIT_PLAYERS,
    WAIT_PLAYERS_READY,
    STARTING,
    RUNNING,
    STOPPING,
    FINISHED,
};
