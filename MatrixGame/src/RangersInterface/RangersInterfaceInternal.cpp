// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "RangersInterfaceInternal.h"
#include "RangersResourcesData.h"

// TODO avoid singleton 
static RangersInterfaceInternal *pInstance = nullptr;

RangersInterfaceInternal::RangersInterfaceInternal() {
    CRangersResourcesData dataCache = CRangersResourcesData{L"cfg\\CacheData.dat"};

    std::map<std::wstring, std::wstring> sounds;
    dataCache.GetSounds(sounds);

    // Unused
    m_sMGDRobotInterface.m_Sound = [](wchar_t *path) {};

    m_sMGDRobotInterface.m_SoundCreate = [](wchar_t *path, int group, int loop) {
        return pInstance->GetSound()->SoundCreate(path, group, loop);
    };

    m_sMGDRobotInterface.m_SoundDestroy = [](uint32_t id) { pInstance->GetSound()->SoundDestroy(id); };
    m_sMGDRobotInterface.m_SoundPlay = [](uint32_t id) { pInstance->GetSound()->SoundPlay(id); };
    m_sMGDRobotInterface.m_SoundIsPlay = [](uint32_t id) { return pInstance->GetSound()->SoundIsPlay(id); };
    m_sMGDRobotInterface.m_SoundVolume = [](uint32_t id, float vol) { pInstance->GetSound()->SoundVolume(id, vol); };
    m_sMGDRobotInterface.m_SoundPan = [](uint32_t id, float pan) { pInstance->GetSound()->SoundPan(id, pan); };
    m_sMGDRobotInterface.m_SoundGetVolume = [](uint32_t id) { return pInstance->GetSound()->SoundGetVolume(id); };
    m_sMGDRobotInterface.m_SoundGetPan = [](uint32_t id) { return pInstance->GetSound()->SoundGetPan(id); };

    m_sMGDRobotInterface.m_RangersText = [](wchar_t *text, wchar_t *font, uint32_t color, int sizex, int sizey,
                                            int alignx, int aligny, int wordwrap, int smex, int smy, Base::CRect *clipr,
                                            SMGDRangersInterfaceText *it) {
        pInstance->GetText()->CreateText(text, font, color, sizex, sizey, alignx, aligny, wordwrap, smex, smy, (RECT*)clipr,
                                         (SMGDRangersInterfaceTextImpl*)it);
    };
    m_sMGDRobotInterface.m_RangersTextClear = [](SMGDRangersInterfaceText *it) {
        pInstance->GetText()->DestroyText((SMGDRangersInterfaceTextImpl*)it);
    };

    m_sMGDRobotInterface.m_ProgressBar = [](float val) { pInstance->GetGame()->SetProgressBar(val); };

    m_sMGDRobotInterface.m_Takt = []() { pInstance->GetGame()->DoTact(); };
    m_sMGDRobotInterface.m_Begin = []() { pInstance->GetGame()->Begin(); };

    m_sMGDRobotInterface.m_MusicVolumeGet = []() { return pInstance->GetGame()->GetMusicVolume(); };
    m_sMGDRobotInterface.m_MusicVolumeSet = [](float v) { pInstance->GetGame()->SetMusicVolume(v); };
}

RangersInterfaceInternal *RangersInterfaceInternal::getInstance() {
    if (!pInstance) {
        pInstance = new RangersInterfaceInternal();
    }
    return pInstance;
}
