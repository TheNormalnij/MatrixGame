// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CCommandAnonymous.h"
#include <cstring>

CCommandAnonymous::CCommandAnonymous(char *data, size_t len) {
    m_len = len;
    m_data = new char[len];
    
    std::memcpy(m_data, data, len);
}

CCommandAnonymous::~CCommandAnonymous() {
    delete m_data;
}
