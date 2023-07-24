// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once
#include "../sessions/ISession.h"

class ITransportHandler {
public:
	virtual void HandlePacket(ISession* session, char *data, size_t len) = 0;
};