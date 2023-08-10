// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CReadyPacket.h"
#include <shared/net/Protocol.h>
#include <shared/net/PacketEnums.h>

void CReadyPacket::WritePacket(CWriteStream *stream) {
    stream->Write((uint8_t)EGamePacketType::READY);
    stream->Write((uint8_t)1);
}
