// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <shared/net/CReadStream.h>
#include <game/IGame.h>
#include <net/sessions/ISession.h>

class IPacketHandler {
public:
    virtual void Handle(CReadStream &stream, ISession *session, IGame *game) = 0;
};