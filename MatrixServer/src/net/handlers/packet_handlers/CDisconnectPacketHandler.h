// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "IPacketHandler.h"

class CDisconnectPacketHandler : public IPacketHandler {
public:
    CDisconnectPacketHandler() = default;
    ~CDisconnectPacketHandler() = default;

    void Handle(CReadStream &stream, ISession *session, IGame *game) override;
};
