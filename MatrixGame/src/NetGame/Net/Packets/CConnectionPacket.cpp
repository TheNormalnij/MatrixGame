// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CConnectionPacket.h"

#include <shared/net/Protocol.h>
#include <shared/net/PacketEnums.h>

void CConnectionPacket::WritePacket(CWriteStream *stream) {
    stream->Write((uint8_t)EGamePacketType::CONNECT);
    stream->Write(PROTOCOL_MAGIC, PROTOCOL_MAGIC_SIZE);
    stream->Write((uint32_t)PROTOCOL_VERSION);
}
