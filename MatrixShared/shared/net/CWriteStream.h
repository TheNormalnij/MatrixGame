// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once
#include <vector>
#include <cstring>
#include <cstdint>

class CWriteStream {
public:
    CWriteStream() = default;
    ~CWriteStream() = default;

    template <class T>
    void Write(T in) {
        const size_t len = sizeof(T);
        const size_t currentPos = m_data.size();
        
        m_data.resize(currentPos + len);
        *(T *)(m_data.data() + currentPos) = in;
    }

    template <class T = void *>
    void Write(T in, size_t len) {
        const size_t currentPos = m_data.size();

        m_data.resize(currentPos + len);
        std::memcpy(m_data.data() + currentPos, in, len);
    }

    char *GetData() const noexcept { return (char*)m_data.data(); };
    size_t GetSize() const noexcept { return m_data.size(); };

private:
    std::vector<uint8_t> m_data;
};
