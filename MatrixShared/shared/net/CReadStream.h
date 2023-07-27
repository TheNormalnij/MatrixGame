// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <cstring>

class CReadStream {
public:
    CReadStream(char *data, size_t len) : m_data(data), m_len(len){};
    ~CReadStream() = default;

    template <class T>
    void Read(T& out) {
        out = *(T *)(m_data + m_currentPos);
        m_currentPos += sizeof(T);
    }

    template <class T>
    void Read(T *out, size_t size) {
        std::memcpy(out, m_data + m_currentPos, size);
        m_currentPos += size;
    }

    void Skip(size_t count) {
        m_currentPos += count;
    }

private:
    size_t m_currentPos = 0;
    size_t m_len;
    char *m_data;

};