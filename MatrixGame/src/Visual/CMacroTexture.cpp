// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CMacroTexture.h"

CMacroTexture::CMacroTexture() {
    m_MacrotextureSize = 1;
    m_Macrotexture = nullptr;
}

CMacroTexture::~CMacroTexture() {}

void CMacroTexture::Init(const std::wstring &path) {
    DTRACE();

    Clear();

    m_Macrotexture = (CTextureManaged *)g_Cache->Get(cc_TextureManaged, path.c_str());

    int cnt = ParamParser{path}.GetCountPar(L"?");
    for (int i = 1; i < cnt; i++) {
        ParamParser op = utils::trim(ParamParser{path}.GetStrPar(i, L"?"));
        if (utils::starts_with(op, L"SIM"))
            m_MacrotextureSize = op.GetInt();
    }
}

void CMacroTexture::Clear() {
    DTRACE();

    if (m_Macrotexture) {
        m_Macrotexture = nullptr;
    }
}

void CMacroTexture::Prepare() {
    if (m_Macrotexture)
        m_Macrotexture->Prepare();
}
