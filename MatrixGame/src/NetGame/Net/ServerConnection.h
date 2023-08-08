// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <string>
#include <string_view>
#include "Clients/IClient.h"
#include "ServerApi.h"

enum class EServerConnectionStatus {
    CONNECTING,
    CONNECTED,
    DISCONNECTED,
    DISCONNECTING,
};

class CServerConnection {
public:
    CServerConnection(std::string_view host, INetworkClient *pTransport, CServerAPI *pServerApi, ITransportDataHandler *pTargetHandler);
    ~CServerConnection() = default;

    void Connect();
    void Reconnect();
    void Disconnect();

    EServerConnectionStatus GetConnectStatus() const noexcept { return m_status; };

private:
    void StartTransport();
    void TryHandshake();
    void ApplyInitialPacketHandler();
    void ApplyMainPacketHandler();
    void RemoveCurrentPacketHandler();

private:
    EServerConnectionStatus m_status;
    std::string m_host;
    INetworkClient *m_pTransport;
    ITransportDataHandler *m_pCurrentDataHandler;
    ITransportDataHandler *m_pTargetDataHandler;
    CServerAPI *m_pServerApi;
};