// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <string_view>
#include <functional>
#include <shared/net/CRequest.h>
#include "../Handlers/ITransportDataHandler.h"

class INetworkClient {
public:
    virtual ~INetworkClient() = default;
    virtual bool Connect(std::string_view adress) = 0;
    virtual bool Close() = 0;
    virtual void SendData(CRequest *req) = 0;
    virtual void DoUpdate() = 0;
    virtual void SetPacketHandler(ITransportDataHandler *handler) = 0;
};
