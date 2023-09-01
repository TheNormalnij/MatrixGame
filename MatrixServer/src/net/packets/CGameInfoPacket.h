// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <shared/net/IPacket.h>
#include <string_view>

class CGameInfoPacket : public IPacket {
public:
    CGameInfoPacket(std::string_view &map, uint32_t seed, uint8_t side, uint8_t aiState)
      : m_playerSide(side), m_sideAiState(aiState), m_map(map), m_seed(seed){};

    ~CGameInfoPacket() = default;

    void WritePacket(CWriteStream *stream) override;

private:
    std::string_view &m_map;
    uint32_t m_seed;
    uint8_t m_playerSide;
    uint8_t m_sideAiState;
};
