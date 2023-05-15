// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "Encryption.hpp"

int32_t nextKey(int32_t key) {
    key = (0x41A7 * (key % 0x1F31D)) - (2836 * (key / 127773));

    if (key <= 0)
        key += 0x7fffffff;

    return key;
}

void encodeOrDecodeData(const char *from, char *to, size_t count, int32_t key) {
    for (uint32_t i = 0; i < count; i++) {
        key = nextKey(key);
        to[i] = (key - 1 & 0xFF ^ (from[i])) & 0xFF;
    }
};
