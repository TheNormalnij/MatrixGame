// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CCommandPacket.h"

#include <shared/net/PacketEnums.h>

void CCommandPacket::WritePacket(CWriteStream *stream) {
    stream->Write((uint8_t)EGamePacketType::COMMANDS);
    stream->Write((uint8_t)1);
    stream->Write((uint16_t)m_command->GetType());
    m_command->Write(*stream);
}
