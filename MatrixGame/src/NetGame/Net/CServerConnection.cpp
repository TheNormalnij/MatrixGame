// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CServerConnection.h"
#include "Clients/ClientTCP.h"
#include "Handlers/CInitialPacketHandler.h"

CServerConnection::CServerConnection(std::string_view host, INetworkClient *pTransport, CServerAPI *pServerApi,
                                     ITransportDataHandler *pTargetHandler) {
    m_host = host;
    m_status = EServerConnectionStatus::DISCONNECTED;
    m_pTransport = pTransport;
    m_pCurrentDataHandler = nullptr;
    m_pTargetDataHandler = pTargetHandler;
    m_pServerApi = pServerApi;
}

void CServerConnection::Connect() {
    if (m_status != EServerConnectionStatus::DISCONNECTED) {
        return;
    }

    m_status = EServerConnectionStatus::CONNECTING;
    StartTransport();
}

void CServerConnection::Reconnect() {}

void CServerConnection::Disconnect() {}

void CServerConnection::StartTransport() {
    ApplyInitialPacketHandler();
    const bool success = m_pTransport->Connect(m_host);
    if (success) {
        TryHandshake();
    }
    else {
        m_status = EServerConnectionStatus::DISCONNECTED;
    } 
}

void CServerConnection::TryHandshake() {
    m_pServerApi->SendConnect();
}

void CServerConnection::ApplyInitialPacketHandler() {
    RemoveCurrentPacketHandler();

    m_pCurrentDataHandler = new CInitialPacketHandler([this](bool succees, const char *error) {
        if (succees) {
            m_status = EServerConnectionStatus::CONNECTED;
            ApplyMainPacketHandler();
        }
        else {
            m_status = EServerConnectionStatus::DISCONNECTED;
        }
    });

    m_pTransport->SetPacketHandler(m_pCurrentDataHandler);
}

void CServerConnection::ApplyMainPacketHandler() {
    RemoveCurrentPacketHandler();
    m_pTransport->SetPacketHandler(m_pTargetDataHandler);
}

void CServerConnection::RemoveCurrentPacketHandler() {
    if (m_pCurrentDataHandler) {
        delete m_pCurrentDataHandler;
    }
}