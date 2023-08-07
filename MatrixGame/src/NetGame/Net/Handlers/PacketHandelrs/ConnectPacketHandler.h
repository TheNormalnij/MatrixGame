// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "../IPacketHandler.h"

class CConnectPacketHandler : public IPacketHandler {
public:
    CConnectPacketHandler() = default;
    ~CConnectPacketHandler() = default;

    virtual void Handle(CReadStream *stream) override;
};