// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "InitialPacketHandler.h"
#include <cstdint>
#include <shared/net/Protocol.h>
#include <shared/net/PacketEnums.h>

void CInitialPacketHandler::Handle(CReadStream *stream) {
    uint8_t packetType;
    stream->Read(packetType);

    if (packetType != (uint8_t)EGamePacketType::CONNECT) {
        m_callback(false, "[CInitialPacketHandler] Server respons with wrong packet type");
        return;
    }

    char magick[13] = {0};
    stream->Read(magick, 12);
    if (std::string_view(PROTOCOL_MAGIC).compare(magick) != 0) {
        m_callback(false, "[CInitialPacketHandler] Server respons with invalid magick symbols");
        return;
    }
    
    uint32_t protocolVersion;
    stream->Read(protocolVersion);

    if (protocolVersion != PROTOCOL_VERSION) {
        m_callback(false, "[CInitialPacketHandler] Protocol version missmath");
        return;
    }

    m_callback(true, nullptr);
}
