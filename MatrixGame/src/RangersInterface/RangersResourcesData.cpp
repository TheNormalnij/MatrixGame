// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "RangersResourcesData.h"
#include "RangersCacheDataLoader.h"

CRangersResourcesData::CRangersResourcesData(const std::wstring_view path) : m_filePath(path) {

}

void CRangersResourcesData::GetSounds(std::map<std::wstring, std::wstring> &outputMap) {
    const CBlockPar *sounds = m_cacheBlockPar.BlockGet(L"Sound");
    BlockParToMap(sounds, outputMap);
}

void CRangersResourcesData::Load() {
    CRangersCacheDataLoader loader;
    loader.Load(L"cfg\\CacheData.dat", m_cacheBlockPar);
}

void CRangersResourcesData::Unload() {
    m_cacheBlockPar.Clear();
}

// TODO use utils class
void CRangersResourcesData::BlockParToMap(const CBlockPar *blockPar, std::map<std::wstring, std::wstring> &outputMap) {
    for (int i = 0; i < blockPar->ParCount(); i++) {
        std::wstring name = blockPar->ParGetName(i);
        std::wstring value = blockPar->ParGet(i);
        outputMap.insert({name, value});
    }
}
