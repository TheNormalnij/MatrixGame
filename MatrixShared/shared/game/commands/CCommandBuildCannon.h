// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "../IGameCommand.h"

class CCommandBuildCannon : public IGameCommand {
public:
    CCommandBuildCannon() : IGameCommand(EGameCommandType::BUILD_CANNON){};
    ~CCommandBuildCannon() = default; 

    // Inherited via IGameCommand
    void Write(CWriteStream &stream) override;
    void Read(CReadStream &stream) override;

public:
    uint8_t side;
    float baseX;
    float baseY;
    float x;
    float y;
    float angle;
    uint16_t place;
    uint8_t cannonId;
};