// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "../IGameCommand.h"
#include "../Game.h"

class CCommandMoveRobots : public IGameCommand {
public:
    CCommandMoveRobots();
    ~CCommandMoveRobots();

    // Inherited via IGameCommand
    void Write(CWriteStream &stream) override;
    void Read(CReadStream &stream) override;

public:
    // This cmd will free this object after use
    uint32_t *robots;
    size_t count;
    uint32_t x;
    uint32_t y;
    uint8_t side;
};
