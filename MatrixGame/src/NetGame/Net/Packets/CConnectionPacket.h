// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include <shared/net/IPacket.h>

class CConnectionPacket : public IPacket {
public:
    CConnectionPacket() = default;
    ~CConnectionPacket() = default;

    void WritePacket(CWriteStream *stream) override;
};