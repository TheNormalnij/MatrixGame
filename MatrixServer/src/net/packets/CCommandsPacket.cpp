// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CCommandsPacket.h"
#include <shared/net/PacketEnums.h>

void CCommandsPacket::WritePacket(CWriteStream *stream) {
    stream->Write((uint8_t)EGamePacketType::COMMANDS);
    for (IGameCommand *command : m_commands) {
        command->Write(*stream);
    }
}