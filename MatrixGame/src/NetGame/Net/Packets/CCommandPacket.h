// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <shared/net/IPacket.h>
#include <shared/game/IGameCommand.h>

class CCommandPacket : public IPacket {
public:
    CCommandPacket(IGameCommand *command) : m_command(command){};
    ~CCommandPacket() = default;

    void WritePacket(CWriteStream *stream) override;

private:
    IGameCommand *m_command;
};