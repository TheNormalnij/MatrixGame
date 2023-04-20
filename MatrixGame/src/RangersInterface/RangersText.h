
#pragma once

#include "../MatrixGameDll.hpp"
#include "stdafx.h"

class RangersText {
public:
    RangersText();
    ~RangersText();

    void CreateText(wchar_t *text, wchar_t *font, uint32_t color, int sizex, int sizey, int alignx, int aligny,
                     int wordwrap, int smex, int smy, Base::CRect *clipr, SMGDRangersInterfaceText *it);

    void DestroyText(SMGDRangersInterfaceText *it);

private:
    void ResetInterface(SMGDRangersInterfaceText *it);
    LPD3DXFONT GetFont(wchar_t* fontName);

private:
    LPD3DXFONT m_Font;
};