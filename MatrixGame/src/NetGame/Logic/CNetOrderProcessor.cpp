// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2023, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CNetOrderProcessor.h"
#include <shared/game/commands/CCommandBuildCannon.h>
#include <shared/game/commands/CCommandBuildRobot.h>
#include <shared/game/commands/CCommandMoveRobots.h>
#include "MatrixRobot.hpp"

void CNetOrderProcessor::BuildTurret(CMatrixBuilding* pParentBase, float posX, float posY, float angle, int place,
    int cannonId) {

    CCommandBuildCannon cmd;
    cmd.side = m_pSide->GetId();
    cmd.x = posX;
    cmd.y = posY;
    cmd.angle = angle;
    cmd.place = place;
    cmd.cannonId = (uint8_t)cannonId;

    m_serializer.SerializeBaseToPos(pParentBase, cmd.baseX, cmd.baseY);

    m_pServerApi->SendCommand(cmd);
}

void CNetOrderProcessor::BuildRobot(CMatrixBuilding *pParentBase, SRobotCostructInfo &info, int count) {
    CCommandBuildRobot cmd;
    cmd.side = m_pSide->GetId();
    cmd.head = info.head;
    cmd.armour = info.armour;
    cmd.chassis = info.chassis;
    for (int i = 0; i < ROBOT_MAX_WEAPONS; i++) {
        cmd.weapon[i] = info.weapon[i];
    }

    cmd.count = count;

    m_serializer.SerializeBaseToPos(pParentBase, cmd.baseX, cmd.baseY);

    m_pServerApi->SendCommand(cmd);
}

void CNetOrderProcessor::MoveRobots(int x, int y, std::list<CMatrixRobotAI *> &list) {
    CCommandMoveRobots cmd;
    cmd.side = m_pSide->GetId();
    cmd.x = x;
    cmd.y = y;
    cmd.count = list.size();
    cmd.robots = new unsigned int[cmd.count]{};
    size_t i = 0;
    for (const auto &robot : list) {
        cmd.robots[i] = robot->GetId();
        i++;
    }

    m_pServerApi->SendCommand(cmd);
}
