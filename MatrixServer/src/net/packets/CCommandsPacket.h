// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <shared/net/IPacket.h>
#include <list>
#include <shared/game/IGameCommand.h>

class CCommandsPacket : public IPacket {
public:
    CCommandsPacket(std::list<IGameCommand *> &commands) : m_commands(commands){};
    ~CCommandsPacket() = default;

     void WritePacket(CWriteStream *stream) override;

private:
    std::list<IGameCommand *> &m_commands;
};