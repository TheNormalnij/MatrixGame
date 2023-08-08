// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CGetGameInfoPacketHandler.h"

void CGetGameInfoPacketHandler::Handle(CReadStream &stream, ISession *session, IGame *game) {
    game->OnAskGameInfo(session);
}