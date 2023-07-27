// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CGamePacketHandlerFactory.h"

#include "packet_handlers/CConnectPacketHandler.h"
#include "packet_handlers/CCommandPacketHandler.h"
#include "packet_handlers/CDisconnectPacketHandler.h"
#include "packet_handlers/CReadyPacketHandler.h"

IPacketHandler *CGamePacketHandlerFactory::CreateHandler(EGamePacketType packetId) {
    switch (packetId) {
        case EGamePacketType::CONNECT:
            return new CConnectPacketHandler();
        case EGamePacketType::DISCONNECT:
            return new CDisconnectPacketHandler();
        case EGamePacketType::COMMANDS:
            return new CCommandPacketHandler();
        case EGamePacketType::READY:
            return new CReadyPacketHandler();
        default:
            return nullptr;
    }
}
