// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "CMatrixMapVisual.h"

class CMatrixVisuaLoader {
public:
    CMatrixVisuaLoader(CMatrixMapVisual *mapVisual) : m_mapVisual(mapVisual){};
    ~CMatrixVisuaLoader() = default;

    void Load(CDataBuf *key, CDataBuf *value);

private: 
    CMatrixMapVisual *m_mapVisual;
};
