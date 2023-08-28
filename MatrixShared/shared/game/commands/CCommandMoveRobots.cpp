// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CCommandMoveRobots.h"

CCommandMoveRobots::CCommandMoveRobots() : IGameCommand(EGameCommandType::ROBOTS_MOVE) {
    robots = nullptr;
    count = 0;
    side = 0;
    x = 0;
    y = 0;
}

CCommandMoveRobots::~CCommandMoveRobots() {
    if (robots) {
        delete robots;
    }
}

void CCommandMoveRobots::Write(CWriteStream &stream) {
    stream.Write(side);
    stream.Write(x);
    stream.Write(y);
    stream.Write(count);
    for (int i = 0; i < count; i++) {
        stream.Write(robots[i]);
    }
}

void CCommandMoveRobots::Read(CReadStream &stream) {
    stream.Read(side);
    stream.Read(x);
    stream.Read(y);
    stream.Read(count);
    robots = new uint32_t[count]{};
    for (int i = 0; i < count; i++) {
        stream.Read(robots[i]);
    }
}
