// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2023, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CObjectSerializer.h"

void CObjectSerializer::SerializeBaseToPos(CMatrixBuilding *pBase, float &x, float &y) const noexcept {
    x = pBase->m_Pos.x;
    y = pBase->m_Pos.y;
}

void CObjectSerializer::DeserializeBaseFromPos(CMatrixBuilding **pBase, float x, float y) const noexcept {
    CMatrixMapStatic *ms = CMatrixMapStatic::GetFirstLogic();

    for (; ms; ms = ms->GetNextLogic()) {
        if (ms->GetObjectType() == OBJECT_TYPE_BUILDING) {
            CMatrixBuilding *pMsBase = (CMatrixBuilding *)ms;
            if (pMsBase->m_Pos.x == x && pMsBase->m_Pos.y == y) {
                *pBase = pMsBase;
                return;
            }
        }
    }
    *pBase = nullptr;
}
