
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

void RangersText::CreateText(wchar_t *text, wchar_t *font, uint32_t color, int sizex, int sizey,
                                           int alignx, int aligny, int wordwrap, int smex, int smy, Base::CRect *clipr,
                                           SMGDRangersInterfaceText *it) {
    int res = 0;

    std::wstring_view strText = std::wstring_view(text);

    LPD3DXFONT pFont = GetFont(font);

    if (!pFont) {
        ResetInterface(it);
        return;
    }

    // Ќужно динамически определ€ть по количеству строк
    if (sizey == 0) {
        size_t linesCount = 1;
        for (const wchar_t c : strText) {
            if (c == '\n') {
                linesCount++;
            }
        }
        sizey = linesCount * 30;
    }

    res = g_D3DD->CreateTexture(sizex, sizey, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &checker, NULL);
    if (FAILED(res)) {
        ResetInterface(it);
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

    pFont->DrawTextW(NULL, text, wcslen(text), &clipRect, format, color);

    checker->UnlockRect(0);
    pSurfaceRender->EndScene(0);

    // Unload resource

    pSurfaceRender->Release();
    surface->Release();

    it->m_Image = (uint32_t)rect;
    it->m_Buf = (uint8_t *)rect->pBits;
    it->m_Pitch = rect->Pitch;
    it->m_SizeX = sizex;
    it->m_SizeY = sizey;
}

void RangersText::DestroyText(SMGDRangersInterfaceText *it) {
    const D3DLOCKED_RECT* rect = (D3DLOCKED_RECT *)it->m_Image;
    if (rect)
        delete rect;

    if (checker) {
        checker->Release();
        checker = nullptr;
    }
}

void RangersText::ResetInterface(SMGDRangersInterfaceText *it) {
    it->m_Image = 0;
    it->m_Buf = 0;
    it->m_Pitch = 0;
    it->m_SizeX = 0;
    it->m_SizeY = 0;
}

LPD3DXFONT RangersText::GetFont(wchar_t *fontName) {
    if (!m_Font) {
        D3DXCreateFontW(g_D3DD, 10, 0, FW_NORMAL, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY,
                        DEFAULT_PITCH, L"MS Sans Serif", &m_Font);
    }

    return m_Font;
}
