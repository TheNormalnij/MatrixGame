// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <string_view>
#include <functional>
#include <shared/net/CRequest.h>
#include "../Handlers/ITransportDataHandler.h"

class INetworkClient;

typedef std::function<void(INetworkClient *client, bool success, std::string_view error)> net_client_close_cb;
typedef std::function<void (INetworkClient *client, bool success, std::string_view error)> net_client_connect_cb;

class INetworkClient {
public:
    virtual ~INetworkClient() = default;
    virtual void Connect(std::string_view adress, net_client_connect_cb callback) = 0;
    virtual void Close(net_client_close_cb callback) = 0;
    virtual void SendData(CRequest *req) = 0;
    virtual void DoUpdate() = 0;
    virtual void SetPacketHandler(ITransportDataHandler *handler) = 0;
};
