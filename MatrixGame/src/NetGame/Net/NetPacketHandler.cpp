// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "NetPacketHandler.h"
#include <shared/net/PacketEnums.h>
#include "Handlers/ConnectPacketHandler.h"

#include <cstdint>

void CNetPacketHandler::Handle(CReadStream *stream) {
    uint8_t packetType;
    stream->Read((&packetType);
    switch ((EGamePacketType)packetType)
	{
        case EGamePacketType::CONNECT: {
            CConnectPacketHandler handler;
            handler.Handle(stream);
            break;
        }
        case EGamePacketType::COMMANDS: {
            
            break;
        }
        default:
            break;
	}
}