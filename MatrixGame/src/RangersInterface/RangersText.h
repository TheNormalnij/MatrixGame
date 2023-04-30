
#pragma once

#include "../MatrixGameDll.hpp"
#include "stdafx.h"
#include "RangersInterfaceTextImpl.h"

class RangersText {
public:
    RangersText();
    ~RangersText();

    void CreateText(std::wstring_view text, wchar_t *font, uint32_t color, int sizex, int sizey, int alignx, int aligny,
                    int wordwrap, int smex, int smy, Base::CRect *clipr, SMGDRangersInterfaceTextImpl *it);

    void DestroyText(SMGDRangersInterfaceTextImpl *it);

private:
    LPD3DXFONT GetFont(wchar_t *fontName);
    void DrawRangersText(std::wstring_view text, LPD3DXFONT font, RECT rect, DWORD format, D3DCOLOR color);
    D3DCOLOR GetColorFromTag(std::wstring_view text, D3DCOLOR defaultColor);

private:
    LPD3DXFONT m_Font;
};