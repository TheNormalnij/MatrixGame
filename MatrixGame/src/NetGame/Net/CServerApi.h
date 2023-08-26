// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <shared/net/IPacket.h>
#include "Clients/IClient.h"
#include <shared/game/IGameCommand.h>

class CServerAPI {
public:
    CServerAPI(INetworkClient *pNetClient) : m_pNetClient(pNetClient){};
    ~CServerAPI();

	void SendConnect();
    void SendAskGameInfo();
    void SendReady();

    template<class T>
    void SendCommands(T commands);

    void SendCommand(IGameCommand &command);

private:
    void Send(IPacket &stream);

private:
    INetworkClient *m_pNetClient;
};
