// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "AskGameInfoPacket.h"
#include <shared/net/Protocol.h>
#include <shared/net/PacketEnums.h>

void CAskGameInfoPacket::WritePacket(CWriteStream *stream) {
    stream->Write((uint8_t)EGamePacketType::GAME_INFO);
}
