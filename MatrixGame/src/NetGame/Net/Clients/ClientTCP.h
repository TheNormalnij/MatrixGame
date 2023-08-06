// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "IClient.h"
#include "../Handlers/IPacketHandler.h"
#include <WinSock2.h>

class CClientTCP : public INetworkClient {
public:
    CClientTCP();
    ~CClientTCP();

    // Inherited via INetworkClient
    virtual void Connect(std::string_view adress, net_client_connect_cb callback) override;
    virtual void Close(net_client_close_cb callback) override;
    virtual void SendData(CRequest *req) override;
    virtual void DoUpdate() override;
    virtual void SetPacketHandler(IPacketHandler *handler) override { m_pPacketHandler = handler; };

private:
    bool CreateSocket(int netType);
    int  GetAdressType(std::string_view adress) const noexcept;

private:
    IPacketHandler *m_pPacketHandler;
    SOCKET m_hSocket;
};