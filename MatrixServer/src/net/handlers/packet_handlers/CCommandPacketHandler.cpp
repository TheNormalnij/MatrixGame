// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CCommandPacketHandler.h"
#include <shared/game/Comands.h>
#include <cstdint>

void CCommandPacketHandler::Handle(CReadStream &stream, ISession *session, IGame *game) {
    uint8_t commandsCount;

    stream.Read(commandsCount);

    for (size_t i; i < commandsCount; i++) {
        ICommandFactory *factory = game->GetCommandFactory();

        IGameCommand *command = factory->CreateCommand(stream);
        if (command) {
            game->HandleCommand(command);
        }
    }
}
