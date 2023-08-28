// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2023, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "MatrixObjectBuilding.hpp"
#include <list>

struct SRobotCostructInfo {
    char head = 0;
    char armour = 0;
    char chassis = 0;
    char weapon[5] = {0};
};

struct SRobotMoveInfo {
    CMatrixRobotAI *robot;
    float x;
    float y;
};

class IOrderProcessor {
public:
    virtual ~IOrderProcessor() = default;
    
    virtual void BuildTurret(CMatrixBuilding *pParentBase, float posX, float posY, float angle, int place,
                             int cannonId) = 0;

    virtual void BuildRobot(CMatrixBuilding *pParentBase, SRobotCostructInfo &info, int count) = 0;
    virtual void MoveRobots(int x, int y, std::list<CMatrixRobotAI*> &list) = 0;
};
