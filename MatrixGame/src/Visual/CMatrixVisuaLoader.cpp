// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CMatrixVisualLoader.h"

void CMatrixVisuaLoader::Load(CDataBuf *propkey, CDataBuf *propval) {
    int ic;

    ic = propkey->FindAsWStr(DATA_SKYCOLOR);
    if (ic >= 0) {
        auto color = propval->GetAsParamParser(ic).GetDword() | 0xFF000000;
        m_mapVisual->SetSkyColor(color);
    }
    else {
        m_mapVisual->SetSkyColor(DEF_SKY_COLOR | 0xFF000000);
    }

    ic = propkey->FindAsWStr(DATA_SHADOWCOLOR);
    if (ic >= 0)
        m_mapVisual->SetShadowColor(propval->GetAsParamParser(ic).GetDword());

    ic = propkey->FindAsWStr(DATA_BIASTER);
    if (ic >= 0)
        m_mapVisual->SetBiasTer((float)propval->GetAsParamParser(ic).GetDouble());

    
    ic = propkey->FindAsWStr(DATA_BIASWATER);
    if (ic >= 0)
        m_mapVisual->SetBiasWater((float)propval->GetAsParamParser(ic).GetDouble());

    ic = propkey->FindAsWStr(DATA_BIASROBOTS);
    if (ic >= 0)
        m_mapVisual->SetBiasRobot((float)propval->GetAsParamParser(ic).GetDouble());

    ic = propkey->FindAsWStr(DATA_AMBIENTCOLOROBJ);
    if (ic >= 0)
        m_mapVisual->SetAmbientObjColor(propval->GetAsParamParser(ic).GetDword());
}
