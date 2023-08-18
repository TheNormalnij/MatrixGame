// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "../IGameCommand.h"

class CCommandBuildRobot : public IGameCommand {
public:
    // Inherited via IGameCommand
    void Write(CWriteStream &stream) override;
    void Read(CReadStream &stream) override;
};