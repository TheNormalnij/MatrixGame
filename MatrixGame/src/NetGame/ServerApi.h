// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <shared/net/IPacket.h>

class CServerAPI {
public:
    CServerAPI();
    ~CServerAPI();

	void SendConnect();
    template<class T>
    void SendCommands(T commands);

private:
    void Send(IPacket &stream);
};
