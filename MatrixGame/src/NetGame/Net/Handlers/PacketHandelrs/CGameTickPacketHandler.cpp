// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CGameTickPacketHandler.h"
#include <cstdint>
#include <vector>
#include <shared/game/ICommandFactory.h>

void CGameTickPacketHandler::Handle(CReadStream *stream, INetGameHandler *game) {
    uint64_t tick;
    stream->Read(tick);
    uint16_t commandsCount;
    stream->Read(commandsCount);
    
    std::vector<IGameCommand*> commands;
    commands.reserve(commandsCount);

    ICommandFactory *factory = game->GetCommandFactory();

    for (size_t i = 0; i < commandsCount; i++) {
        factory->CreateCommand(*stream);
    }

    game->OnGetTickCommands(tick, commands);
}