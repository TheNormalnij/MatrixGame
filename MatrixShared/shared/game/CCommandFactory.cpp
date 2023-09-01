// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CCommandFactory.h"

#include "Comands.h"
#include "commands/CCommandBuildRobot.h"
#include "commands/CCommandBuildCannon.h"
#include "commands/CCommandMoveRobots.h"

IGameCommand *CCommandFactory::CreateCommand(CReadStream &stream) {
    uint16_t commandId;
    stream.Read(commandId);

    IGameCommand *command = CreateCommand(commandId);
    
    if (command) {
        command->Read(stream);
    }

    return command;
}

IGameCommand *CCommandFactory::CreateCommand(uint16_t commandId) noexcept {
    switch ((EGameCommandType)commandId) {
        case EGameCommandType::BUILD_ROBOT: {
            return new CCommandBuildRobot();
        }
        case EGameCommandType::BUILD_CANNON: {
            return new CCommandBuildCannon();
        }
        case EGameCommandType::ROBOTS_MOVE: {
            return new CCommandMoveRobots();
        }
        default:
            return nullptr;
    }
}
