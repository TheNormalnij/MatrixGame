// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CCommandPacketHandler.h"
#include "game/commands/CCommandAnonymous.h"
#include <shared/game/Comands.h>
#include <cstdint>

void CCommandPacketHandler::Handle(CReadStream &stream, ISession *session, IGame *game) {
    uint8_t commandsCount;

    stream.Read(commandsCount);

    for (size_t i; i < commandsCount; i++) {
        uint8_t commandType;

        stream.Read(commandType);

        switch ((EGmaeCommandType)commandType) {
            case EGmaeCommandType::ANONYMOUS: {
                uint16_t commandSize;
                stream.Read(commandSize);

                char *commandData;
                stream.Read(commandData, commandSize);

                IGameCommand *command = new CCommandAnonymous(commandData, commandSize);

                game->HandleCommand(command);
                break;
            }
            default:
                break;
        }
    }
}
