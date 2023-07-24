// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "IPacketHandler.h"

class CCommandPacketHandler : public IPacketHandler {
public:
    CCommandPacketHandler() = default;
    ~CCommandPacketHandler() = default;

    void Handle(CBitstream &stream, ISession *session, IGame *game) override;
};
