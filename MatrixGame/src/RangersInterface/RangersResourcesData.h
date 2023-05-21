// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <string_view>
#include <string>
#include <map>
#include "CBlockPar.hpp"

class CRangersResourcesData {
public:
    CRangersResourcesData(const std::wstring_view path);
    ~CRangersResourcesData() = default;

    void GetSounds(std::map<std::wstring, std::wstring> &outputMap);
    void Load();
    void Unload();

private:
    void BlockParToMap(const CBlockPar *blockPar, std::map<std::wstring, std::wstring> &outputMap);

private:
    const std::wstring_view m_filePath;
    CBlockPar m_cacheBlockPar;
};
