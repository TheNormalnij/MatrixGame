// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "RangersSound.h"

#define INVALID_SOUND_ID -1
const size_t MAX_SOUNDS_COUNT = 16;

//   TODO
// * Fix sound pan
// * Fix sound loop
// * Make it safe

struct SWavHeader {
    // RIFF header
    uint8_t riffHeader[4];    // Contains "RIFF"
    uint32_t fileSize;        // Size of the entire file minus 8 bytes

    // WAVE header
    uint8_t waveHeader[4];    // Contains "WAVE"

    // Format subchunk
    uint8_t fmtHeader[4];     // Contains "fmt "
    uint32_t fmtSize;         // Size of the format subchunk
    uint16_t audioFormat;     // Audio format (1 = PCM)
    uint16_t numChannels;     // Number of channels (1 = mono, 2 = stereo)
    uint32_t sampleRate;      // Sample rate (in Hz)
    uint32_t byteRate;        // Bytes per second
    uint16_t blockAlign;      // Block alignment
    uint16_t bitsPerSample;   // Bits per sample

                              // Unknown chunks here
    // Data subchunk
    uint8_t dataHeader[4];    // Contains "data"
    uint32_t dataSize;        // Size of the data subchunk
};

RangersSound::RangersSound() {
    m_pMasterVoice = nullptr;
    m_pXAudio2 = nullptr;
    m_sounds.resize(MAX_SOUNDS_COUNT, nullptr);
}

RangersSound::~RangersSound() {
    if (m_pXAudio2) {
        Deinit();
    }
}

void RangersSound::Init(std::map<std::wstring, std::wstring> &sounds) {
    // Initialize COM
    #pragma warning(push)
    #pragma warning(disable : 6031)
    CoInitializeEx(NULL, COINIT_MULTITHREADED);
    #pragma warning(pop)

    // Create an XAudio2 engine
    XAudio2Create(&m_pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);

    m_pXAudio2->StartEngine();

    m_pXAudio2->CreateMasteringVoice(&m_pMasterVoice);

    for (const auto pair : sounds) {
        Base::CBuf *buf = new Base::CBuf(NULL);
        buf->LoadFromFile(pair.second);

        m_soundsWavData[pair.first] = buf;
    }
}

void RangersSound::Deinit() {
    if (m_pXAudio2) {
        m_pXAudio2->StopEngine();
        m_pXAudio2->Release();
        m_pXAudio2 = nullptr;
    }

    for (const auto pair : m_soundsWavData) {
        delete pair.second;
    }

    m_soundsWavData.clear();
}

uint32_t RangersSound::GetFreeSoundSlot() const {
    for (size_t i = 0; i < MAX_SOUNDS_COUNT; i++) {
        if (m_sounds[i] == nullptr)
            return i;
    }

    return INVALID_SOUND_ID;
}

void RangersSound::InitWavSoundBuffer(std::wstring_view key, XAUDIO2_BUFFER &bufferData) {
    Base::CBuf *buf = m_soundsWavData[key.data()];

    BYTE *pDataStart = buf->Buff<BYTE>() + 36;
    while (pDataStart[0] != 'd' || pDataStart[1] != 'a' || pDataStart[2] != 't' || pDataStart[3] != 'a') {
        pDataStart += 1;
    }

    bufferData.Flags = 0;  // Either 0 or XAUDIO2_END_OF_STREAM.
    bufferData.AudioBytes = *(uint32_t *)(pDataStart + 4);
    bufferData.pAudioData = pDataStart + 8;
    bufferData.PlayBegin = 0;
    bufferData.PlayLength = 0; // Whole buffer
    bufferData.LoopBegin = 0;
    bufferData.LoopLength = 0;
    bufferData.LoopCount = 0; // XAUDIO2_LOOP_INFINITE to loop forever.
    bufferData.pContext = buf;
}

uint32_t RangersSound::SoundCreate(wchar_t *soundName, int group, int loop) {
    uint32_t slot = GetFreeSoundSlot();
    if (slot == INVALID_SOUND_ID) {
        return INVALID_SOUND_ID;
    }

    if (!std::wstring_view(soundName).starts_with(L"Sound.")) {
        return INVALID_SOUND_ID;
    }

    // Skip "Sound." part
    soundName += 6;

    XAUDIO2_BUFFER bufferData = {0};
    InitWavSoundBuffer(soundName, bufferData);

    bufferData.LoopCount = loop;

    SWavHeader *fileHeader = ((Base::CBuf *)bufferData.pContext)->Buff<SWavHeader>();

    WAVEFORMATEX waveFormat = {0};
    waveFormat.wFormatTag = fileHeader->audioFormat;
    waveFormat.nChannels = fileHeader->numChannels;
    waveFormat.nSamplesPerSec = fileHeader->sampleRate;
    waveFormat.wBitsPerSample = fileHeader->bitsPerSample;
    waveFormat.nBlockAlign = fileHeader->blockAlign;
    waveFormat.nAvgBytesPerSec = fileHeader->byteRate;

    IXAudio2SourceVoice *pSourceVoice;
    if (m_pXAudio2->CreateSourceVoice(&pSourceVoice, &waveFormat) != S_OK)
        return INVALID_SOUND_ID;

    if (pSourceVoice->SubmitSourceBuffer(&bufferData, nullptr) != S_OK)
        return INVALID_SOUND_ID;

    m_sounds[slot] = pSourceVoice;

    return slot;
}

void RangersSound::SoundDestroy(uint32_t id) {
    if (id != INVALID_SOUND_ID && m_sounds[id]) {
        m_sounds[id]->DestroyVoice();
        m_sounds[id] = nullptr;
    }
}

void RangersSound::SoundPlay(uint32_t id) {
    if (id != INVALID_SOUND_ID && m_sounds[id]) 
        m_sounds[id]->Start(0);
}

int RangersSound::SoundIsPlay(uint32_t id) {
    if (id != INVALID_SOUND_ID && m_sounds[id]) {
        XAUDIO2_VOICE_STATE state;
        m_sounds[id]->GetState(&state);
        return state.BuffersQueued;
    }
    return 0;
}

void RangersSound::SoundVolume(uint32_t id, float vol) {
    if (id != INVALID_SOUND_ID && m_sounds[id])
        m_sounds[id]->SetVolume(vol);
}

float RangersSound::SoundGetVolume(uint32_t id) {
    if (id != INVALID_SOUND_ID && m_sounds[id]) {
        float volume;
        m_sounds[id]->GetVolume(&volume);
        return volume;
    }

    return 0;
}

void RangersSound::SoundPan(uint32_t id, float pan) {
    if (id != INVALID_SOUND_ID && m_sounds[id]) {
        XAUDIO2_VOICE_DETAILS voiceDetails;
        m_sounds[id]->GetVoiceDetails(&voiceDetails);

        XAUDIO2_VOICE_DETAILS masterVoiceDetails;
        m_sounds[id]->GetVoiceDetails(&masterVoiceDetails);

        // TODO it's wrong
        float matrix[8] = {0};
        matrix[0] = 0.5f - pan / 2;
        matrix[1] = 0.5f + pan / 2;

        m_sounds[id]->SetOutputMatrix(nullptr, voiceDetails.InputChannels, masterVoiceDetails.InputChannels, matrix);
    }
 }

float RangersSound::SoundGetPan(uint32_t id) {
    if (id != INVALID_SOUND_ID && m_sounds[id]) {
        XAUDIO2_VOICE_DETAILS VoiceDetails;
        m_sounds[id]->GetVoiceDetails(&VoiceDetails);

        XAUDIO2_VOICE_DETAILS MasterVoiceDetails;
        m_sounds[id]->GetVoiceDetails(&MasterVoiceDetails);

        float output[8];
        m_sounds[id]->GetOutputMatrix(nullptr, VoiceDetails.InputChannels, MasterVoiceDetails.InputChannels, output);
        return (output[0] - 0.5f) / -2;
    }

    return 0;
 }
