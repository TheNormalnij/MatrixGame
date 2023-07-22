// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "PacketEnums.h"

#pragma once

class CBitstream {
public:
    CBitstream(char *data, size_t len) : m_data(data), m_len(len) {};
    ~CBitstream() = default;

    template <class T>
    void Read(T& out) {
        out = *(T *)(m_data + m_currentPos);
        m_currentPos += sizeof(T);
    }

    template <class T>
    void Write(T in) {
        *(m_data + m_currentPos) = in;
        m_currentPos += sizeof(T);
    }

private:
    size_t m_currentPos = 0;
    size_t m_len;
    char *m_data;

};