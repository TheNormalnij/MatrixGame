
#include "RangersText.h"
#include <stdio.h>

IDirect3DTexture9 *checker;

RangersText::RangersText() {
    m_Font = nullptr;
}

RangersText::~RangersText() {
    if (m_Font) {
        m_Font->Release();
    }
}

void RangersText::CreateText(std::wstring_view text, wchar_t *font, uint32_t color, int sizex, int sizey, int alignx,
                             int aligny, int wordwrap, int smex, int smy, Base::CRect *clipr,
                             SMGDRangersInterfaceTextImpl *it) {
    int res = 0;

    LPD3DXFONT pFont = GetFont(font);

    if (!pFont) {
        it->Reset();
        return;
    }

    if (sizey == 0) {
        size_t linesCount = 1;
        for (const wchar_t c : text) {
            if (c == '\n') {
                linesCount++;
            }
        }
        sizey = linesCount * 30;
    }

    res = g_D3DD->CreateTexture(sizex, sizey, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &checker, NULL);
    if (FAILED(res)) {
        it->Reset();
        return;
    }

    IDirect3DSurface9 *surface;

    checker->GetSurfaceLevel(0, &surface);

    D3DLOCKED_RECT *rect = new D3DLOCKED_RECT();
    checker->LockRect(0, rect, NULL, D3DUSAGE_WRITEONLY);

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

    RECT clipRect = *(RECT *)clipr;
    clipRect.left += smex;
    clipRect.right += smy;

    DrawRangersText(text, pFont, clipRect, format, color);

    checker->UnlockRect(0);
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

    if (checker) {
        checker->Release();
        checker = nullptr;
    }
}

LPD3DXFONT RangersText::GetFont(wchar_t *fontName) {
    if (!m_Font) {
        D3DXCreateFontW(g_D3DD, 10, 0, FW_NORMAL, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY,
                        DEFAULT_PITCH, L"MS Sans Serif", &m_Font);
    }

    return m_Font;
}

void RangersText::DrawRangersText(std::wstring_view text, LPD3DXFONT pFont, RECT rect, DWORD format, D3DCOLOR color) {
    size_t drawFrom = 0;
    const size_t drawEnd = text.size();
    size_t drawTo;

    D3DCOLOR currentColor = color;

    while (drawFrom < drawEnd) {
        const size_t tagOpen = text.find(L"<", drawFrom);

        bool needDraw = true;

        if (tagOpen == std::wstring::npos) {
            drawTo = drawEnd;
        }
        else {
            if (tagOpen == drawFrom) {
                currentColor = GetColorFromTag(&text[drawFrom], color);
                needDraw = false;

                const size_t tagClose = text.find(L">", tagOpen);
                if (tagClose != std::wstring::npos) {
                    drawFrom = tagClose;
                    drawTo = drawFrom;
                }
                else {
                    drawTo = drawFrom - 1;
                }
            }
            else {
                drawTo = tagOpen;
            }
        }

        if (needDraw) {
            pFont->DrawTextW(NULL, &text[drawFrom], drawTo - drawFrom, &rect, format, currentColor);
        }

        if (tagOpen == std::wstring::npos) {
            drawFrom = drawTo + 1;
        }
        else {
            //const size_t tagClose = text.find(L">", tagOpen);
            //if (tagClose == std::wstring::npos) {
            //    drawFrom = drawTo + 1;
            //}
            //else {
            //    drawFrom = tagClose + 1;
            //}

            drawFrom = drawTo + 1;
        }
    }
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
