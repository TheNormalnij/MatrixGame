// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CGameNetDataHandler.h"
#include <shared/net/PacketEnums.h>
#include "PacketHandelrs/ConnectPacketHandler.h"

#include <cstdint>

void CGameNetDataHandler::Handle(CReadStream *stream) {
    uint8_t packetType;
    stream->Read(packetType);
    switch ((EGamePacketType)packetType)
	{
        case EGamePacketType::CONNECT: {
            CConnectPacketHandler handler;
            handler.Handle(stream, m_pGame);
            break;
        }
        case EGamePacketType::GAME_INFO: {
            CConnectPacketHandler handler;
            handler.Handle(stream, m_pGame);
            break;
        }
        case EGamePacketType::COMMANDS: {
            
            break;
        }
        default:
            break;
	}
}