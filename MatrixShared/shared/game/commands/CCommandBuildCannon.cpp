// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CCommandBuildCannon.h"

void CCommandBuildCannon::Write(CWriteStream &stream) {
    stream.Write(side);
    stream.Write(baseX);
    stream.Write(baseY);
    stream.Write(x);
    stream.Write(y);
    stream.Write(angle);
    stream.Write(place);
    stream.Write(cannonId);
}

void CCommandBuildCannon::Read(CReadStream &stream) {
    stream.Read(side);
    stream.Read(baseX);
    stream.Read(baseY);
    stream.Read(x);
    stream.Read(y);
    stream.Read(angle);
    stream.Read(place);
    stream.Read(cannonId);
}
