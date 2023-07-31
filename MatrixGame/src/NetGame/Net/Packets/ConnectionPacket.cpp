// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "ConnectionPacket.h"
#include <shared/net/Protocol.h>

void CConnectionPacket::WritePacket(CWriteStream *stream) {
    stream->Write(PROTOCOL_MAGIC, 12);
    stream->Write((uint32_t)PROTOCOL_VERSION);
}
