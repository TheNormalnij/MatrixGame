
#pragma once
#include <cstdint>

class RangersSound {
public:
    RangersSound() = default;
    ~RangersSound() = default;

    uint32_t SoundCreate(wchar_t *path, int group, int loop);
    void SoundDestroy(uint32_t id);
    void SoundPlay(uint32_t id);
    int SoundIsPlay(uint32_t id);
    void SoundVolume(uint32_t id, float vol);
    void SoundPan(uint32_t id, float pan);
    float SoundGetVolume(uint32_t id);
    float SoundGetPan(uint32_t id);
};