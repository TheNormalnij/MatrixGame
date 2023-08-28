// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2023, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CLocalOrderProcessor.h"

#include "MatrixObjectCannon.hpp"
#include "Interface/CConstructor.h"

 CLocalOrderProcessor::CLocalOrderProcessor(CMatrixSideUnit *side) {
    m_pSide = side;
 }

void CLocalOrderProcessor::BuildTurret(CMatrixBuilding *pParentBase, float posX, float posY, float angle, int place,
                                       int cannonId) {
    CMatrixCannon *ca = g_MatrixMap->StaticAdd<CMatrixCannon>(true);
    ca->m_CurrState = CANNON_UNDER_CONSTRUCTION;

    ca->SetInvulnerability();
    ca->m_Pos.x = posX;
    ca->m_Pos.y = posY;
    ca->m_Place = place;
    ca->SetSide(m_pSide->GetId());
    ca->UnitInit(cannonId);

    ca->m_Angle = angle;
    ca->m_AddH = 0;

    ca->m_ShadowType = SHADOW_STENCIL;
    ca->m_ShadowSize = 128;

    ca->RNeed(MR_Matrix | MR_Graph);
    ca->m_ParentBuilding = pParentBase;
    ca->JoinToGroup();

    pParentBase->m_TurretsHave++;
    ca->SetHitPoint(0);
    pParentBase->m_BS.AddItem(ca);

    TakeResources(g_Config.m_CannonsProps[cannonId - 1].m_Resources);
}

void CLocalOrderProcessor::BuildRobot(CMatrixBuilding *pParentBase, SRobotCostructInfo &info, int count) {
    CConstructor *constructor = m_pSide->GetConstructor();

    constructor->SetBase(pParentBase);
    constructor->SetSide(m_pSide->GetId());
    constructor->OperateUnit(MRT_CHASSIS, (ERobotUnitKind)info.chassis);
    constructor->OperateUnit(MRT_ARMOR, (ERobotUnitKind)info.armour);
    constructor->OperateUnit(MRT_HEAD, (ERobotUnitKind)info.head);

    for (int i = 0; i < MAX_WEAPON_CNT; i++) {
        if (info.weapon[i] > 0) {
            constructor->Djeans007(MRT_WEAPON, (ERobotUnitKind)info.weapon[i], i);
        }
    }

    for (int i = 0; i < count; i++) {
        constructor->StackRobot(nullptr);
    }

    int res[MAX_RESOURCES];
    constructor->GetConstructionPrice(res);

    for (size_t i = 0; i < ERes::MAX_RESOURCES; i++) {
        res[i] *= count;
    }

    TakeResources(res);
}

void CLocalOrderProcessor::MoveRobots(int mx, int my, std::list<CMatrixRobotAI*> &list) {
    const int logicId = m_pSide->GetFreeLogicGroup();

    for (const auto& robot : list) {
        m_pSide->AddRobotToLogicGroup(logicId, robot);
    }

    m_pSide->PGOrderMoveTo(logicId, CPoint(mx - ROBOT_MOVECELLS_PER_SIZE / 2, my - ROBOT_MOVECELLS_PER_SIZE / 2));
}

void CLocalOrderProcessor::TakeResources(int *res) {
    m_pSide->AddResourceAmount(TITAN, -res[TITAN]);
    m_pSide->AddResourceAmount(ELECTRONICS, -res[ELECTRONICS]);
    m_pSide->AddResourceAmount(ENERGY, -res[ENERGY]);
    m_pSide->AddResourceAmount(PLASMA, -res[PLASMA]);
}
