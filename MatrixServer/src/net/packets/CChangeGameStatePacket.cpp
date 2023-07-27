
// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CChangeGameStatePacket.h"
#include <shared/net/PacketEnums.h>

void CChangeGameStatePacket::WritePacket(CWriteStream *stream) {
    stream->Write((uint8_t)EGamePacketType::GAME_STATUS_CHANGED);
    stream->Write((uint8_t)m_status);
}
