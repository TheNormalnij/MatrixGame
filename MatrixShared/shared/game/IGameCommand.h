// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "../net/CWriteStream.h"
#include "../net/CReadStream.h"

#include "Comands.h"

class IGameCommand {
public:
    IGameCommand(EGameCommandType type) : m_Type(type){};
    virtual ~IGameCommand() = default;
    virtual void Write(CWriteStream &stream) = 0;
    virtual void Read(CReadStream &stream) = 0;

    EGameCommandType GetType() const noexcept { return m_Type; };

private:
    EGameCommandType m_Type;
};
