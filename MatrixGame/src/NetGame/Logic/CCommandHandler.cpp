// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CCommandHandler.h"

#include "CObjectSerializer.h"
#include <cassert>

void CCommandHandler::Handle(IGameCommand *cmd) {
    switch (cmd->GetType()) {
        case EGameCommandType::BUILD_CANNON: {
            BuildCannonHandle((CCommandBuildCannon*)cmd);
            break;
        }
        default:
            break;
    }
}

void CCommandHandler::BuildCannonHandle(CCommandBuildCannon *cmd) {
    CObjectSerializer serializer;

    CMatrixBuilding *pBase = nullptr;
    serializer.DeserializeBaseFromPos(&pBase, cmd->baseX, cmd->baseY);

    assert(pBase);

    CLocalOrderProcessor orderProcessor = GetSideOrderProcessor(cmd->side);
    orderProcessor.BuildTurret(pBase, cmd->x, cmd->y, cmd->angle, cmd->place, cmd->cannonId);
}

CLocalOrderProcessor CCommandHandler::GetSideOrderProcessor(int sideId) {
    assert(sideId <= m_sidesCount);
    return CLocalOrderProcessor(&m_pSides[sideId]);
}
