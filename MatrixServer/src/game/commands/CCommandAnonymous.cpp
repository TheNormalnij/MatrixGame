// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CCommandAnonymous.h"
#include <shared/game/Comands.h>
#include <cstring>

CCommandAnonymous::CCommandAnonymous(char *data, size_t len) {
    m_len = len;
    m_data = new char[len];
    
    std::memcpy(m_data, data, len);
}

CCommandAnonymous::~CCommandAnonymous() {
    delete m_data;
}

void CCommandAnonymous::Write(CWriteStream &stream) {
    stream.Write((uint8_t)EGmaeCommandType::ANONYMOUS);
    stream.Write((uint16_t)m_len);
    stream.Write(m_data, m_len);
}

void CCommandAnonymous::Read(CReadStream &stream) {
    stream.Read((uint16_t&)m_len);

    m_data = new char[m_len];
    stream.Read(m_data, m_len);
}
