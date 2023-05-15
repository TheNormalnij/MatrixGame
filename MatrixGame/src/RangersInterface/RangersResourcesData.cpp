// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "RangersResourcesData.h"
#include "RangersCacheDataLoader.h"

const int32_t CACHE_ENC_KEY = 0xEA8F3F37;

CRangersResourcesData::CRangersResourcesData(const std::wstring_view path) : m_filePath(path) {

}

void CRangersResourcesData::GetSounds(std::map<std::wstring, std::wstring> &outputMap) {
    CRangersCacheDataLoader loader;
    CBlockPar data;

    loader.Load(L"cfg\\CacheData.dat", data);

    const CBlockPar * sounds = data.BlockGet(L"Sound");

    for (int i = 0; i < sounds->ParCount(); i++) {
        std::wstring name = sounds->ParGetName(i);
        std::wstring value = sounds->ParGet(i);
        outputMap.insert({name, value});
    }
}

void CRangersResourcesData::Load() {

}


void CRangersResourcesData::Unload() {

}