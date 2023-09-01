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
    virtual bool Connect(std::string_view adress) override;
    virtual bool Close() override;
    virtual void SendData(CRequest *req) override;
    virtual void DoUpdate() override;
    virtual void SetPacketHandler(ITransportDataHandler *handler) override { m_pPacketHandler = handler; };

private:
    bool EnableNetwork();
    bool CreateSocket(int netType);
    int  GetAdressType(std::string_view adress) const noexcept;

private:
    ITransportDataHandler *m_pPacketHandler;
    SOCKET m_hSocket;
};