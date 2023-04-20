
#include "RangersGame.h"
#include <stdio.h>

void RangersGame::SetProgressBar(float val) {
    printf("[STUB] SetProgressBar %f\n", (double)val);
}

void RangersGame::Begin() {
    printf("[STUB] Begin\n");
}

void RangersGame::DoTact() {
    printf("[STUB] DoTact\n");
}

float RangersGame::GetMusicVolume() {
    printf("[STUB] GetMusicVolume returned 0.0\n ");
    return 0.0f;
}

void RangersGame::SetMusicVolume(float volume) {
    printf("[STUB] SetMusicVolume %f\n", (double)volume);
}