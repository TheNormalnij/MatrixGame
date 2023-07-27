// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "IPacket.h"
#include <list>
#include "game/commands/IGameCommand.h"

class CCommandsPacket : public IPacket {
public:
    CCommandsPacket(std::list<IGameCommand *> &commands) : m_commands(commands){};

     void WritePacket(CWriteStream *stream) override;

private:
    std::list<IGameCommand *> &m_commands;
};