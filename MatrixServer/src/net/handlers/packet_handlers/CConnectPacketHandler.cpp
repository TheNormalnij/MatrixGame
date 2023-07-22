// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CConnectPacketHandler.h"

void CConnectPacketHandler::Handle(CBitstream &stream, ISession *session, IGame *game) {
    game->OnRequestPlayerJoin(session);
}
