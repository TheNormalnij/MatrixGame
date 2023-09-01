// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

enum class EGamePacketType {
    CONNECT,
    DISCONNECT,
    READY,
    COMMANDS,
    GAME_STATUS_CHANGED,
    GAME_INFO,
};
