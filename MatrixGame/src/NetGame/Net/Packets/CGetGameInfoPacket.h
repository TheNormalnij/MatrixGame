// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include <shared/net/IPacket.h>

class CGetGameInfoPacket : public IPacket {
public:
    CGetGameInfoPacket() = default;
    ~CGetGameInfoPacketHandler() = default;

    void WritePacket(CWriteStream *stream) override;
};