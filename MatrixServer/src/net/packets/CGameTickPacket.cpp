// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CGameTickPacket.h"
#include <shared/net/PacketEnums.h>

void CGameTickPacket::WritePacket(CWriteStream *stream) {
    stream->Write((uint8_t)EGamePacketType::COMMANDS);
    stream->Write((uint64_t)m_tick);
    stream->Write((uint16_t)m_commands.size());

    for (IGameCommand *command : m_commands) {
        command->Write(*stream);
    }
}
