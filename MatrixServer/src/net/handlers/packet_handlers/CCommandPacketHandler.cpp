// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CCommandPacketHandler.h"
#include <game/commands/CCommandAnonimous.h>
#include <shared/game/Comands.h>
#include <cstdint>

void CCommandPacketHandler::Handle(CBitstream &stream, ISession *session, IGame *game) {
    const uint8_t commandsCount;

    stream.Read(commandsCount);

    for (size_t i; i < commandsCount; i++) {
        const uint8_t commandType;

        stream.Read(commandType);

        switch ((EGmaeCommandType)commandType) {
            case EGmaeCommandType::ANONYMOUS:
                const uint16_t commandSize;
                stream.Read(commandSize);

                const uint8_t *commandData;
                stream.Read(commandData, commandSize);

                const IGameCommand *command = new CCommandAnonimous(commandData, commandSize);

                game->HandleCommand(command);
            default:
                break;
        }
    }
}
