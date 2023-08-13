// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CGameInfoPacketHandler.h"
#include <cstdint>
#include <string>

void CGameInfoPacketHandler::Handle(CReadStream *stream, INetGameHandler *game) {
    uint32_t stringLen;
    stream->Read(stringLen);
    std::string mapName;
    mapName.resize(stringLen);
    stream->Read(mapName.c_str(), stringLen);

    uint32_t seed;
    stream->Read(seed);

    game->OnGetGameInfo(mapName, seed);
}