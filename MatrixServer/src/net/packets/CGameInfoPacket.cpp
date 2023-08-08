// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CGameInfoPacket.h"
#include <shared/net/PacketEnums.h>

void CGameInfoPacket::WritePacket(CWriteStream *stream) {
    stream->Write((uint8_t)EGamePacketType::GAME_INFO);
    const uint32_t stringLen = m_map.size();
    stream->Write(stringLen);
    stream->Write(m_map.data(), stringLen);
    stream->Write(m_seed);
}