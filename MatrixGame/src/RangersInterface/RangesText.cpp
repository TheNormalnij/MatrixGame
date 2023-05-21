// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "RangersText.h"
#include <stdio.h>
#include <list>
#include "3g.hpp"

RangersText::RangersText() {
    m_Font = nullptr;
    m_Texture = nullptr;
}

RangersText::~RangersText() {
    if (m_Font) {
        m_Font->Release();
    }
}

void RangersText::CreateText(std::wstring_view text, wchar_t *font, uint32_t color, int sizex, int sizey, int alignx,
                             int aligny, int wordwrap, int smex, int smy, RECT *clipr,
                             SMGDRangersInterfaceTextImpl *it) {
    int res = 0;

    LPD3DXFONT pFont = GetFont(font);

    if (!pFont) {
        it->Reset();
        return;
    }

    if (sizey == 0) {
        sizey = GetTextHeight(text, pFont, *clipr);
    }

    // Prepare texture
    res = g_D3DD->CreateTexture(sizex, sizey, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_Texture,
                                NULL);
    if (FAILED(res)) {
        it->Reset();
        return;
    }

    IDirect3DSurface9 *surface;

    m_Texture->GetSurfaceLevel(0, &surface);

    D3DLOCKED_RECT *rect = new D3DLOCKED_RECT();
    m_Texture->LockRect(0, rect, NULL, D3DUSAGE_WRITEONLY);

    LPD3DXRENDERTOSURFACE pSurfaceRender = nullptr;

    D3DXCreateRenderToSurface(g_D3DD, sizex, sizey, D3DFMT_A8R8G8B8, 0, D3DFMT_UNKNOWN, &pSurfaceRender);

    pSurfaceRender->BeginScene(surface, NULL);

    // Render text

    DWORD format = DT_NOCLIP;
    switch (alignx) {
        case 1:
            format = format | DT_CENTER;
            break;
        case 2:
            format = format | DT_RIGHT;
            break;
        default:
            format = format | DT_LEFT;
            break;
    }

    switch (aligny) {
        case 1:
            format = format | DT_VCENTER;
            break;
        case 2:
            format = format | DT_BOTTOM;
            break;
        default:
            format = format | DT_TOP;
            break;
    }

    if (wordwrap)
        format = format | DT_WORDBREAK;

    RECT clipRect = *clipr;
    clipRect.left += smex;
    clipRect.right += smy;

    DrawRangersText(text, pFont, clipRect, format, color);

    m_Texture->UnlockRect(0);
    pSurfaceRender->EndScene(0);

    // Unload resource

    pSurfaceRender->Release();
    surface->Release();

    it->SetRectData(rect, sizex, sizey);
}

void RangersText::DestroyText(SMGDRangersInterfaceTextImpl *it) {
    const D3DLOCKED_RECT *rect = it->GetRect();
    if (rect)
        delete rect;

    if (m_Texture) {
        m_Texture->Release();
        m_Texture = nullptr;
    }
}

LPD3DXFONT RangersText::GetFont(wchar_t *fontName) {
    if (!m_Font) {
        D3DXCreateFontW(g_D3DD, 10, 0, FW_NORMAL, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY,
                        DEFAULT_PITCH, L"MS Sans Serif", &m_Font);
    }

    return m_Font;
}

int RangersText::GetFontHeight(LPD3DXFONT pDXFont) {
    D3DXFONT_DESCW desc;
    pDXFont->GetDescW(&desc);
    return desc.Height;
}

int RangersText::GetTextHeight(std::wstring_view text, LPD3DXFONT pFont, RECT &rect) {
    std::wstring textWithouTags;
    AppendTextWithoutTags(text, textWithouTags);

    DWORD format = DT_CALCRECT | DT_WORDBREAK;

    RECT resRect = rect;

    pFont->DrawTextW(NULL, textWithouTags.c_str(), textWithouTags.size(), &resRect, format, 0);

    return resRect.bottom - resRect.top;
}

void RangersText::AppendTextWithoutTags(std::wstring_view text, std::wstring &resultString) {
    bool insideTag = false;
    for (const wchar_t c : text) {
        if (c == L'<') {
            insideTag = true;
        }
        else if (c == L'>') {
            insideTag = false;
        }
        else if (!insideTag) {
            resultString += c;
        }
    }
}

void RangersText::DrawRangersText(std::wstring_view text, LPD3DXFONT pFont, RECT &rect, DWORD format, D3DCOLOR color) {

    struct STextSection {
        const wchar_t *textStart;
        int textCount;
        int width;
        D3DCOLOR color;
    };

    struct SLine {
        std::list<STextSection> lineSections;
        int width = 0;
    };

    std::list<SLine> lineList;

    lineList.push_back(SLine());
    SLine &currentLine = lineList.back();

    const int lineHeight = GetFontHeight(pFont) + 3;
    const int lineMaxWidth = rect.right - rect.left;
    const int spaceSize = GetFontSpaceSize(pFont);

    const bool wordWrap = true; 

    // Break text into sections
    int currentY = 0;
    int lineWidth = 0;
    D3DCOLOR currentColor = color;
    const wchar_t *wszSectionPos = text.data();
    do {
        unsigned int uiSeekPos = 0;
        const wchar_t *wszSectionStart = wszSectionPos;
        D3DCOLOR nextColor = currentColor;
        bool startNewLine = false;
        while (*wszSectionPos != '\0')  // find end of this section
        {
            if (*wszSectionPos == '<') {
                nextColor = GetColorFromTag(wszSectionPos, color);
                size_t nextPos = ((std::wstring_view)wszSectionPos).find(L'>', 7);
                wszSectionPos += nextPos + 1;
                break;
            }
            else if (*wszSectionPos == '\r') {
                startNewLine = true;
                wszSectionPos += 2;
                break;
            }
            wszSectionPos++;
            uiSeekPos++;
        }

        if (uiSeekPos > 0) {
            const int width = GetTextWidth(wszSectionStart, uiSeekPos, pFont);

            if (lineWidth + width <= lineMaxWidth) {
                lineList.back().lineSections.push_back(STextSection());
                STextSection &section = lineList.back().lineSections.back();
                section.textStart = wszSectionStart;
                section.textCount = uiSeekPos;
                section.color = currentColor;

                section.width = width;

                lineWidth += width;
            }
            else {
                const wchar_t* lineStart = wszSectionStart;
                const wchar_t* wordStart = lineStart;
                int lineWidthAtStart = lineWidth;
                const size_t addSize = 0;
                size_t sectionWidth = 0;

                size_t pos = 0;
                while (pos < uiSeekPos) {
                    size_t wordSize = std::wstring_view(wordStart).find(L' ', 0);
                    wordSize = wordSize == -1 ? uiSeekPos - pos : wordSize;

                    const int maxWordSize = uiSeekPos - pos;
                    if (wordSize > maxWordSize)
                        wordSize = maxWordSize;

                    const int wordWidth = GetTextWidth(wordStart, wordSize, pFont);
                    if (lineWidth == 0) {
                        // Continue this line, cause this is empty
                        lineWidth = wordWidth;
                        sectionWidth = lineWidth;
                        pos += wordSize + 1;
                        wordStart += wordSize + 1;
                    }
                    else if (lineWidth + wordWidth > lineMaxWidth) {
                        // Start new line
                        lineList.back().lineSections.push_back(STextSection());
                        STextSection &section = lineList.back().lineSections.back();
                        section.textStart = lineStart;
                        section.textCount = wordStart - lineStart;
                        section.color = currentColor;
                        section.width = sectionWidth;

                        lineList.back().width = lineWidth;
                        lineList.push_back(SLine());

                        currentY += lineHeight;
                        lineWidth = 0;
                        sectionWidth = 0;
                        lineWidthAtStart = 0;

                        //pos += endI + 1;
                        //wordStart += endI + 1;
                        lineStart = wordStart;
                    }
                    else {
                        // Continue this line
                        sectionWidth = GetTextWidth(lineStart, wordStart - lineStart + wordSize, pFont);
                        lineWidth = lineWidthAtStart + sectionWidth;
                        pos += wordSize + 1;
                        wordStart += wordSize + 1;
                    }
                }

                lineList.back().lineSections.push_back(STextSection());
                STextSection &section = lineList.back().lineSections.back();
                section.textStart = lineStart;
                section.textCount = wszSectionStart - lineStart + uiSeekPos;
                section.color = currentColor;
    
                if (lineStart[section.textCount - 1] == L' ') {
                    section.width = sectionWidth + spaceSize;
                }
                else {
                    section.width = sectionWidth;
                }
            }
        }

        if (startNewLine) {
            lineList.back().width = lineWidth;
            lineList.push_back(SLine());

            currentY += lineHeight;
            lineWidth = 0;
        }

        currentColor = nextColor;
    }
    while (*wszSectionPos != '\0');

    lineList.back().width = lineWidth;
    const int textHeight = currentY + lineHeight;

    int iTop;
    if (format & DT_VCENTER)
        iTop = rect.top + ((rect.bottom - rect.top) - textHeight) / 2;
    else if (format & DT_BOTTOM)
        iTop = rect.bottom - textHeight;
    else
        iTop = rect.top;

    // Draw all the color sections
    for (const SLine &line : lineList) {
        int iLeft;
        if (format & DT_RIGHT)
            iLeft = rect.right - line.width;
        else if (format & DT_CENTER)
            iLeft = rect.left + ((rect.right - rect.left) - line.width) / 2;
        else
            iLeft = rect.left;

        for (const STextSection &section : line.lineSections) {

            RECT sectionRect{iLeft, iTop, iLeft, iTop + lineHeight};

            pFont->DrawTextW(NULL, section.textStart, section.textCount, &sectionRect, DT_NOCLIP,
                             section.color);

            iLeft += section.width;
        }

        iTop += lineHeight;
    }
}

int RangersText::GetTextWidth(const std::wstring_view wszText, const size_t textLength, const LPD3DXFONT pDXFont) {
    if (wszText[0] == L'\0')
        return 0;

    if (pDXFont) {
        // DT_CALCRECT may not take space characters at the end of a line into consideration for the rect size.
        // Count the amount of space characters at the end
        size_t spaceCount = 0;

        for (int i = textLength - 1; i >= 0; --i) {
            const wchar_t c = wszText[i];

            if (c == L' ')
                ++spaceCount;
            else
                break;
        }

        // Compute the size of a single space and use that to get the width of the ignored space characters
        size_t trailingSpacePixels = 0;

        if (spaceCount > 0) {
            trailingSpacePixels = GetFontSpaceSize(pDXFont) * spaceCount;
        }

        RECT rect = {};

        if ((textLength - spaceCount) > 0) {
            pDXFont->DrawTextW(nullptr, wszText.data(), textLength - spaceCount, &rect, DT_CALCRECT | DT_SINGLELINE,
                               D3DCOLOR_XRGB(0, 0, 0));
        }

        return rect.right - rect.left + trailingSpacePixels;
    }

    return 0;
}

int RangersText::GetFontSpaceSize(const LPD3DXFONT pFont) {
    SIZE size = {};
    GetTextExtentPoint32W(pFont->GetDC(), L" ", 1, &size);
    return size.cx;
}

D3DCOLOR RangersText::GetColorFromTag(std::wstring_view text, D3DCOLOR defaultColor) {
    if ((text[1] == L'c' || text[1] == L'C') && (text[2] == L'o' || text[2] == L'O') &&
        (text[3] == L'l' || text[3] == L'L') && (text[4] == L'o' || text[4] == L'O') &&
        (text[5] == L'r' || text[5] == L'R') && (text[6] == L'=')) {

        try {
            const size_t rEnd = text.find(L',', 7);
            const size_t bEnd = text.find(L',', rEnd + 1);

            std::size_t pos{};

            const int a = 255;
            const int r = std::stoi(&text[7], &pos);
            const int g = std::stoi(&text[rEnd + 1], &pos);
            const int b = std::stoi(&text[bEnd + 1], &pos);

            return a << 24 | r << 16 | g << 8 | b;
        }
        catch (...) {
            return defaultColor;
        }
    }
    return defaultColor;
}
