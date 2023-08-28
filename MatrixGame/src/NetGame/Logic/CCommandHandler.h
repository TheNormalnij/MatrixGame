// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <shared/game/IGameCommand.h>
#include "Control/CLocalOrderProcessor.h"

#include <shared/game/commands/CCommandBuildCannon.h>
#include <shared/game/commands/CCommandBuildRobot.h>
#include <shared/game/commands/CCommandMoveRobots.h>

class CCommandHandler {
public:
    CCommandHandler(CMatrixSideUnit *sides, size_t sidesCount) : m_pSides(sides), m_sidesCount(sidesCount){};
    ~CCommandHandler() = default;

    void Handle(IGameCommand *cmd);

private:
    void BuildCannonHandle(CCommandBuildCannon *cmd);
    void BuildRobotHandle(CCommandBuildRobot *cmd);
    void MoveRobotsHandle(CCommandMoveRobots *cmd);

    CLocalOrderProcessor GetSideOrderProcessor(int sideId);

private:
    CMatrixSideUnit *m_pSides;
    size_t m_sidesCount;
};
