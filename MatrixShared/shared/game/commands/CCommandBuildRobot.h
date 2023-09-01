// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "../IGameCommand.h"
#include "../Game.h"

class CCommandBuildRobot : public IGameCommand {
public:
    CCommandBuildRobot();
    ~CCommandBuildRobot() = default; 

    // Inherited via IGameCommand
    void Write(CWriteStream &stream) override;
    void Read(CReadStream &stream) override;

public:
    uint8_t side;
    float baseX;
    float baseY;
    uint8_t count;
    uint8_t head;
    uint8_t armour;
    uint8_t chassis;
    uint8_t weapon[ROBOT_MAX_WEAPONS];
};