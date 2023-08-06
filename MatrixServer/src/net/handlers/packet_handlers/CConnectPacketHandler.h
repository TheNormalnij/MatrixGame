// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <shared/net/IPacketHandler.h>

class CConnectPacketHandler : public IPacketHandler {
public:
    CConnectPacketHandler() = default;
    ~CConnectPacketHandler() = default;

    void Handle(CReadStream &stream, ISession *session, IGame *game) override;
};