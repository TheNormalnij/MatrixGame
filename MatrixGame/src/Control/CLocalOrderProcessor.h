// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2023, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "IOrderProcessor.h"

// Класс для обработки приказов игрока в рамках локальной игры

class CLocalOrderProcessor : public IOrderProcessor {
public:
    CLocalOrderProcessor(CMatrixSideUnit *side);
    ~CLocalOrderProcessor() = default;

    // IOrderProcessor interface

    void BuildTurret(CMatrixBuilding *pParentBase, float posX, float posY, float angle, int place, int m_cannonId) override;
    void BuildRobot(CMatrixBuilding *pParentBase, SRobotCostructInfo &info, int count) override;

private:
    // TODO: use resource class and resource struct
    void TakeResources(int *res);

private:
    CMatrixSideUnit *m_pSide;
};