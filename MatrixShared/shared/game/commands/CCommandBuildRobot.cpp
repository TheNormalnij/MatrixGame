// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CCommandBuildRobot.h"

CCommandBuildRobot::CCommandBuildRobot() : IGameCommand(EGameCommandType::BUILD_ROBOT) {
    side = 0;
    baseX = 0;
    baseY = 0;
    count = 0;
    head = 0;
    armour = 0;
    chassis = 0;
    weapon[0] = 0;
    weapon[1] = 0;
    weapon[2] = 0;
    weapon[3] = 0;
    weapon[4] = 0;
}

void CCommandBuildRobot::Write(CWriteStream &stream) {
    stream.Write(side);
    stream.Write(baseX);
    stream.Write(baseY);
    stream.Write(count);
    stream.Write(head);
    stream.Write(armour);
    stream.Write(chassis);
    stream.Write(weapon, ROBOT_MAX_WEAPONS);
}

void CCommandBuildRobot::Read(CReadStream &stream) {
    stream.Read(side);
    stream.Read(baseX);
    stream.Read(baseY);
    stream.Read(count);
    stream.Read(head);
    stream.Read(armour);
    stream.Read(chassis);
    stream.Read(weapon, ROBOT_MAX_WEAPONS);
}
