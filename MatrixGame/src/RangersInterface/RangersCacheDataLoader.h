// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <string_view>
#include "CBlockPar.hpp"
#include "CBuf.hpp"

class CRangersCacheDataLoader {
public:
    CRangersCacheDataLoader() = default;
    ~CRangersCacheDataLoader() = default;

    void Load(const std::wstring_view filePath, Base::CBlockPar &out);
    void Load(Base::CBuf &buf, Base::CBlockPar &out);

private:
    void GetClearBuffer(const char* in, size_t in_sise, Base::CBuf &out, uint32_t key);
    void LoadData(Base::CBuf &buf, Base::CBlockPar *out);
};