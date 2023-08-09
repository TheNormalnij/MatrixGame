// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CConnectPacketHandler.h"
#include <shared/net/Protocol.h>

void CConnectPacketHandler::Handle(CReadStream &stream, ISession *session, IGame *game) {
    char header[PROTOCOL_MAGIC_SIZE + 1] = {0};

    stream.Read(header, PROTOCOL_MAGIC_SIZE);

    const std::string_view should;
    if (!std::string_view(header).starts_with(PROTOCOL_MAGIC)) {
        return;
    }

    uint32_t protocolVersion;
    stream.Read(protocolVersion);

    if (protocolVersion != PROTOCOL_VERSION) {
        return;
    }

    game->OnRequestSessionStart(session);
}
