// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CAlwaysDrawStorage.h"

CAlwaysDrawStorage::CAlwaysDrawStorage() {
    m_AD_Obj = {0};
    m_AD_Obj_cnt = 0;
}

void CAlwaysDrawStorage::Add(CMatrixMapStatic *ms) {
    m_AD_Obj[m_AD_Obj_cnt] = ms;
    m_AD_Obj_cnt++;
}

void CAlwaysDrawStorage::Remove(CMatrixMapStatic *ms) {
    for (int i = 0; i < m_AD_Obj_cnt; ++i) {
        if (m_AD_Obj[i] == ms) {
            m_AD_Obj[i] = m_AD_Obj[--m_AD_Obj_cnt];
            return;
        }
    }
}
