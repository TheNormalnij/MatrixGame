// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <shared/net/IPacket.h>
#include <list>
#include <shared/game/IGameCommand.h>

class CGameTickPacket : public IPacket {
public:
    CGameTickPacket(size_t tick, std::list<IGameCommand *> *commands) : m_tick(tick), m_pCommands(commands){};
    ~CGameTickPacket() = default;

     void WritePacket(CWriteStream *stream) override;

private:
    size_t m_tick;
    std::list<IGameCommand *> *m_pCommands;
};