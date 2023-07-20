// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "../../../../MatrixShared/src/net/CBitsream.h"

class IPacketHandler {
    virtual void Handle(CBitstream *stream) = 0;
};