// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <shared/net/CReadStream.h>

class CNetPacketHandler {
public:
    CNetPacketHandler();
    ~CNetPacketHandler();

    void Handle(CReadStream *stream);
};