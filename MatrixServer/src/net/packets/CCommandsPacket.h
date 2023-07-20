// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <cstdint>
#include <list>
#include "../../game/IGameCommand.h"

class CCommandsPacket {
public:
    CCommandPacket(std::list<IGameCommand *> commands) : m_commands(commands){};

    char* Pack();

private:
    std::list<IGameCommand *> m_commands;
};