// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <shared/net/IPacket.h>
#include <shared/game/Game.h>

class CChangeGameStatePacket : public IPacket {
public:
    CChangeGameStatePacket(EGameStatus status) : m_status(status){};
    ~CChangeGameStatePacket() = default;

    void WritePacket(CWriteStream *stream) override;

private:
    EGameStatus &m_status;
};