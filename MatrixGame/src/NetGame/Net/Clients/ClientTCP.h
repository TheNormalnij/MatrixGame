// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "IClient.h"
#include "../IPacketHandler.h"

class CClientTCP : public INetworkClient {
public:
    CClientTCP() = default;
    ~CClientTCP() = default;

    // Inherited via INetworkClient
    virtual void Connect(std::string_view adress, net_client_connect_cb callback) override;
    virtual void Close(std::function<net_client_close_cb> callback) override;
    virtual void SendData(CRequest *req) override;

private:
    IPacketHandler *m_pPacketHandler;
};