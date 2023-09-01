// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CGameNetDataHandler.h"
#include <shared/net/PacketEnums.h>
#include "PacketHandelrs/CConnectPacketHandler.h"
#include "PacketHandelrs/CGameInfoPacketHandler.h"
#include "PacketHandelrs/CGameStatusChangePacketHandler.h"
#include "PacketHandelrs/CGameTickPacketHandler.h"

#include <cstdint>
#include <cassert>

void CGameNetDataHandler::Handle(CReadStream *stream) {
    while (stream->HasData()) {
        uint8_t packetType;
        stream->Read(packetType);
        switch ((EGamePacketType)packetType) {
            case EGamePacketType::CONNECT: {
                CConnectPacketHandler handler;
                handler.Handle(stream, m_pGame);
                break;
            }
            case EGamePacketType::GAME_INFO: {
                CGameInfoPacketHandler handler;
                handler.Handle(stream, m_pGame);
                break;
            }
            case EGamePacketType::GAME_STATUS_CHANGED: {
                CGameStatusChangePacketHandler handler;
                handler.Handle(stream, m_pGame);
                break;
            }
            case EGamePacketType::COMMANDS: {
                CGameTickPacketHandler handler;
                handler.Handle(stream, m_pGame);
                break;
            }
            default:
                assert("Got invalid data");
                break;
        }
    }
}