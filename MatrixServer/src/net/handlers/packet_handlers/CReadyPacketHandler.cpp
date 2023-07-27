// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CReadyPacketHandler.h"
#include "game/players/IPlayer.h"


void CReadyPacketHandler::Handle(CReadStream &stream, ISession *session, IGame *game) {
    IPlayer *player = (IPlayer*)session->GetCustomData();
    game->OnPlayerReady(player);
}