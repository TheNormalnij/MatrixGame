// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2023, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CLocalOrderProcessor.h"

#include "MatrixObjectCannon.hpp"

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

void CLocalOrderProcessor::TakeResources(int *res) {
    m_pSide->AddResourceAmount(TITAN, -res[TITAN]);
    m_pSide->AddResourceAmount(ELECTRONICS, -res[ELECTRONICS]);
    m_pSide->AddResourceAmount(ENERGY, -res[ENERGY]);
    m_pSide->AddResourceAmount(PLASMA, -res[PLASMA]);
}
