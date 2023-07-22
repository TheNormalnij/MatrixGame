// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CGamePacketHandler.h"
#include <net/CBitsream.h>
#include <cstdint>

void CGamePacketHandler::HandlePacket(ISession *session, char *data, size_t len) {
    CBitstream stream = CBitstream(data, len);
    const EGamePacketType packetId = (EGamePacketType)stream.Read<uint8_t>();

    IPacketHandler* handler = m_handlerFactory.CreateHandler(packetId);

    if (!handler) {
        //printf("Got unsupported packet id");
        return;
    }

    handler->Handle(stream, session, m_game);

    delete handler;
}
