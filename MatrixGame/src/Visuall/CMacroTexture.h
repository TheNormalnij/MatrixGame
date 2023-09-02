// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "Texture.hpp"

class CMacroTexture {
public:
    CMacroTexture();
    ~CMacroTexture();

    void Init(const std::wstring &path);
    void Clear();
    void Prepare();

    bool HasTexture() const noexcept { return m_Macrotexture != nullptr; };

    int GetSize() const noexcept { return m_MacrotextureSize; };

    auto Tex() const noexcept { return m_Macrotexture->Tex(); };

private:
    int m_MacrotextureSize;
    CTextureManaged *m_Macrotexture;
};
