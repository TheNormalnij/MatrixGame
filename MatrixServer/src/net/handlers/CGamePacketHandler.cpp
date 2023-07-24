// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CGamePacketHandler.h"
#include <shared/net/CBitsream.h>
#include <cstdint>
#include <ctime>

void CGamePacketHandler::HandlePacket(ISession *session, char *data, size_t len) {
    session->SetLastClientUpdateTimestamp(std::time(nullptr));

    CBitstream stream = CBitstream(data, len);

    uint8_t packetId;
    stream.Read(packetId);

    IPacketHandler* handler = m_handlerFactory.CreateHandler((EGamePacketType)packetId);

    if (!handler) {
        printf("Got unsupported packet id: %d", packetId);
        return;
    }

    handler->Handle(stream, session, m_game);

    delete handler;
}
