// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "IPacket.h"
#include <string_view>

class CGameInfoPacket : public IPacket {
public:
    CGameInfoPacket(std::string_view &map) : m_map(map){};

    void WritePacket(CWriteStream *stream) override;

private:
    std::string_view &m_map;
};