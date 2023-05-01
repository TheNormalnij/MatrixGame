
#pragma once

#include "../MatrixGameDll.hpp"
#include "stdafx.h"
#include "RangersInterfaceTextImpl.h"

class RangersText {
public:
    RangersText();
    ~RangersText();

    void CreateText(std::wstring_view text, wchar_t *font, uint32_t color, int sizex, int sizey, int alignx, int aligny,
                    int wordwrap, int smex, int smy, RECT *clipr, SMGDRangersInterfaceTextImpl *it);

    void DestroyText(SMGDRangersInterfaceTextImpl *it);

private:
    LPD3DXFONT GetFont(wchar_t *fontName);
    void DrawRangersText(std::wstring_view text, LPD3DXFONT font, RECT &rect, DWORD format, D3DCOLOR color);
    D3DCOLOR GetColorFromTag(std::wstring_view text, D3DCOLOR defaultColor);
    int GetFontHeight(LPD3DXFONT pFont);
    int GetTextHeight(std::wstring_view text, LPD3DXFONT pFont, RECT &rect);
    int GetTextWidth(const std::wstring_view wszText, const size_t length, const LPD3DXFONT pFont);
    int GetFontSpaceSize(const LPD3DXFONT pFont);
    void AppendTextWithoutTags(std::wstring_view text, std::wstring &resultString);

private:
    LPD3DXFONT m_Font;
    IDirect3DTexture9 *m_Texture;
};