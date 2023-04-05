
#include "RangersSound.h"

uint32_t RangersSound::SoundCreate(wchar_t *path, int group, int loop) {
    return 0;
}

void RangersSound::SoundDestroy(uint32_t id) {}

void RangersSound::SoundPlay(uint32_t id) {}

int RangersSound::SoundIsPlay(uint32_t id) {
    return 0;
}

void RangersSound::SoundVolume(uint32_t id, float vol) {}

void RangersSound::SoundPan(uint32_t id, float pan) {}

float RangersSound::SoundGetVolume(uint32_t id) {
    return 0.0f;
}

float RangersSound::SoundGetPan(uint32_t id) {
    return 0.0f;
}
