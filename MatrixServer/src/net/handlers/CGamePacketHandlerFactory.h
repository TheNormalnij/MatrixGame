// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "packet_handlers/IPacketHandler.h"

class CGamePacketHandlerFactory {
public:
    CGamePacketHandlerFactory() = default;
    ~CGamePacketHandlerFactory() = default;

    IPacketHandler *CreateHandler(EGamePacketType packetId);
};