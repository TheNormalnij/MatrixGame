// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "RangersInterfaceInternal.h"
#include "RangersResourcesData.h"

// TODO avoid singleton 
static RangersInterfaceInternal *pInstance = nullptr;

RangersInterfaceInternal::RangersInterfaceInternal() {
    LinkInterface();
}

void RangersInterfaceInternal::LoadResources(std::wstring_view resourceLang) {
    // Internal interface
    CFile::AddPackFile(L"DATA\\Sound.pkg", NULL);

    std::wstring soundLocaled = std::wstring(L"DATA\\voices") + resourceLang.data() + L".pkg";
    CFile::AddPackFile(soundLocaled.c_str(), NULL);

    CFile::OpenPackFiles();

    CRangersResourcesData dataCache = CRangersResourcesData{L"cfg\\CacheData.dat"};
    dataCache.Load();

    std::map<std::wstring, std::wstring> gameSounds;
    dataCache.GetSounds(gameSounds);

    m_soundInterface.Init(gameSounds);

    CFile::ReleasePackFiles();
}

void __stdcall _Sound(wchar_t *path) {}

uint32_t __stdcall _SoundCreate(wchar_t* path, int group, int loop) {
    return pInstance->GetSound()->SoundCreate(path, group, loop);
}

void __stdcall _SoundDestroy(uint32_t id) {
    pInstance->GetSound()->SoundDestroy(id);
}

void __stdcall _SoundPlay(uint32_t id) {
    pInstance->GetSound()->SoundPlay(id);
}

int __stdcall _SoundIsPlay(uint32_t id) {
    return pInstance->GetSound()->SoundIsPlay(id);
}

void __stdcall _SoundVolume(uint32_t id, float vol) {
    pInstance->GetSound()->SoundVolume(id, vol);
}
void __stdcall _SoundPan(uint32_t id, float pan) {
    pInstance->GetSound()->SoundPan(id, pan);
}

float __stdcall _SoundGetVolume(uint32_t id) {
    return pInstance->GetSound()->SoundGetVolume(id);
}

float __stdcall _SoundGetPan(uint32_t id) {
    return pInstance->GetSound()->SoundGetPan(id);
}

void __stdcall _CreateText(wchar_t *text, wchar_t *font, uint32_t color, int sizex, int sizey, int alignx, int aligny,
                           int wordwrap, int smex, int smy, Base::CRect *clipr, SMGDRangersInterfaceText *it) {
    pInstance->GetText()->CreateText(text, font, color, sizex, sizey, alignx, aligny, wordwrap, smex, smy,
                                     (RECT *)clipr, (SMGDRangersInterfaceTextImpl *)it);
}

void __stdcall _DestroyText(SMGDRangersInterfaceText *it) {
    pInstance->GetText()->DestroyText((SMGDRangersInterfaceTextImpl *)it);
}

void __stdcall _ProgressBar(float val) {
    pInstance->GetGame()->SetProgressBar(val);
}

void __stdcall _GameTakt() {
    pInstance->GetGame()->DoTact();
}

void __stdcall _GameBegin() {
    pInstance->GetGame()->Begin();
}

float __stdcall _GameMusicGetVolume() {
    return pInstance->GetGame()->GetMusicVolume();
}

void __stdcall _GameMusicSetVolume(float v) {
    pInstance->GetGame()->SetMusicVolume(v);
}

void RangersInterfaceInternal::LinkInterface() {
    m_sMGDRobotInterface.m_Sound = _Sound;
    m_sMGDRobotInterface.m_SoundCreate = _SoundCreate;
    m_sMGDRobotInterface.m_SoundDestroy = _SoundDestroy;
    m_sMGDRobotInterface.m_SoundPlay = _SoundPlay;
    m_sMGDRobotInterface.m_SoundIsPlay = _SoundIsPlay;
    m_sMGDRobotInterface.m_SoundVolume = _SoundVolume;
    m_sMGDRobotInterface.m_SoundPan = _SoundPan;
    m_sMGDRobotInterface.m_SoundGetVolume = _SoundGetVolume;
    m_sMGDRobotInterface.m_SoundGetPan = _SoundGetPan;

    m_sMGDRobotInterface.m_RangersText = _CreateText;
    m_sMGDRobotInterface.m_RangersTextClear = _DestroyText;

    m_sMGDRobotInterface.m_ProgressBar = _ProgressBar;
    m_sMGDRobotInterface.m_Takt = _GameTakt;
    m_sMGDRobotInterface.m_Begin = _GameBegin;
    m_sMGDRobotInterface.m_MusicVolumeGet = _GameMusicGetVolume;
    m_sMGDRobotInterface.m_MusicVolumeSet = _GameMusicSetVolume;
}


RangersInterfaceInternal *RangersInterfaceInternal::getInstance() {
    if (!pInstance) {
        pInstance = new RangersInterfaceInternal();
    }
    return pInstance;
}
