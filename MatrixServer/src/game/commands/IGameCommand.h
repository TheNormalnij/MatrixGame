// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <shared/net/CWriteStream.h>
#include <shared/net/CReadStream.h>

enum class ECommandSourceType {
    SERVER,
    PLAYER,
};

class IGameCommand {
public:
    virtual ~IGameCommand() = 0;
    virtual void Write(CWriteStream &stream) = 0;
    virtual void Read(CReadStream &stream) = 0;
};
