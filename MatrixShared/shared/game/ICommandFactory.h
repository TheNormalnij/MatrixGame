// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "IGameCommand.h"
#include "../net/CReadStream.h"

class ICommandFactory {
public:
    virtual IGameCommand *CreateCommand(CReadStream &stream) = 0;
};
