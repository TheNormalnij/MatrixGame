// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once
#include <cstdint>
#include <map>
#include <string>

class RangersSound {
public:
    RangersSound();
    ~RangersSound();

    uint32_t SoundCreate(wchar_t *path, int group, int loop);
    void SoundDestroy(uint32_t id);
    void SoundPlay(uint32_t id);
    int SoundIsPlay(uint32_t id);
    void SoundVolume(uint32_t id, float vol);
    void SoundPan(uint32_t id, float pan);
    float SoundGetVolume(uint32_t id);
    float SoundGetPan(uint32_t id);

private:
    std::map<std::wstring, std::wstring> m_soundPathes;
};