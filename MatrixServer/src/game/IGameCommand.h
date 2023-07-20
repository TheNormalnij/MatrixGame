// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

enum class ECommandSourceType {
    SERVER,
    PLAYER,
};

class IGameCommand {
    virtual void Serialize() = 0;
    virtual void *GetSource() = 0;
    virtual ECommandSourceType GetSourceType() = 0;
};