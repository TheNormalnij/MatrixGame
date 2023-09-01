// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CGameStatusChangePacketHandler.h"
#include <cstdint>
#include <string>

void CGameStatusChangePacketHandler::Handle(CReadStream *stream, INetGameHandler *game) {
    uint8_t status;
    stream->Read(status);
    game->OnChangeGameState((EGameStatus)status);
}