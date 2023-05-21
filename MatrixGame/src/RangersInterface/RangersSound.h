// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once
#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include <xaudio2.h>
#include "CBuf.hpp"

class RangersSound {
public:
    RangersSound();
    ~RangersSound();

    void Init(std::map<std::wstring, std::wstring> &sounds);
    void Deinit();

    uint32_t SoundCreate(wchar_t *soundName, int group, int loop);
    void SoundDestroy(uint32_t id);
    void SoundPlay(uint32_t id);
    int SoundIsPlay(uint32_t id);
    void SoundVolume(uint32_t id, float vol);
    void SoundPan(uint32_t id, float pan);
    float SoundGetVolume(uint32_t id);
    float SoundGetPan(uint32_t id);
   
private:
    uint32_t GetFreeSoundSlot() const;
    void InitWavSoundBuffer(std::wstring_view key, XAUDIO2_BUFFER &buffData);

private:
    std::vector<IXAudio2SourceVoice*> m_sounds;
    IXAudio2 *m_pXAudio2;
    IXAudio2MasteringVoice *m_pMasterVoice;

    std::map<std::wstring, Base::CBuf*> m_soundsWavData;
};