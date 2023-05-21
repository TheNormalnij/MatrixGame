// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "../MatrixGameDll.hpp"
#include "d3dx9.h"

class SMGDRangersInterfaceTextImpl : protected SMGDRangersInterfaceText {
public:
    void Reset() {
        m_Image = 0;
        m_Buf = 0;
        m_Pitch = 0;
        m_SizeX = 0;
        m_SizeY = 0;
    };

    void SetRectData(D3DLOCKED_RECT *rect, int sizex, int sizey) {
        m_Image = (uint32_t)rect;
        m_Buf = (uint8_t *)rect->pBits;
        m_Pitch = rect->Pitch;
        m_SizeX = sizex;
        m_SizeY = sizey;
    };

    D3DLOCKED_RECT *GetRect() { return (D3DLOCKED_RECT *)m_Image; };
};