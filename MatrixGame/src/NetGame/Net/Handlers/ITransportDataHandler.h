// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <shared/net/CReadStream.h>
#include "INetGameHandler.h"

class ITransportDataHandler {
public:
    virtual ~ITransportDataHandler() = default;
    virtual void Handle(CReadStream *stream) = 0;
};
