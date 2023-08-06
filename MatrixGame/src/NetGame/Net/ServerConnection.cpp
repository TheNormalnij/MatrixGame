// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "ServerConnection.h"
#include "Clients/ClientTCP.h"
#include "Handlers/InitialPacketHandler.h"
#include "Handlers/NetPacketHandler.h"

CServerConnection::CServerConnection(std::string_view host, INetworkClient *pTransport, CServerAPI *pServerApi) {
    m_host = host;
    m_status = EServerConnectionStatus::DISCONNECTED;
    m_pTransport = pTransport;
    m_pCurrentPacketHandler = nullptr;
    m_pServerApi = pServerApi;
}

void CServerConnection::Connect() {
    if (m_status != EServerConnectionStatus::DISCONNECTED) {
        return;
    }

    m_status == EServerConnectionStatus::CONNECTING;
    StartTransport();
}

void CServerConnection::StartTransport() {
     net_client_connect_cb cb = [this](INetworkClient *client, bool success, std::string_view error) {
        if (success) {
           TryHandshake();
        }
        else {
            m_status = EServerConnectionStatus::DISCONNECTED;
        }
        m_status = EServerConnectionStatus::CONNECTED;
    };

    m_pTransport->SetPacketHandler();
    m_pTransport->Connect(m_host, cb);
}

void CServerConnection::TryHandshake() {
    m_pServerApi->SendConnect();
}

void CServerConnection::ApplyInitialPacketHandler() {
    RemoveCurrentPacketHandler();

    m_pCurrentPacketHandler = new CInitialPacketHandler([this](bool succees, const char *error) {
        if (succees) {
            m_status = EServerConnectionStatus::CONNECTED;
            ApplyMainPacketHandler();
        }
        else {
            m_status = EServerConnectionStatus::DISCONNECTED;
        }
    });

    m_pTransport->SetPacketHandler(m_pCurrentPacketHandler);
}

void CServerConnection::ApplyMainPacketHandler() {
    RemoveCurrentPacketHandler();

    m_pCurrentPacketHandler = new CNetPacketHandler();
    m_pTransport->SetPacketHandler(m_pCurrentPacketHandler);
}

void CServerConnection::RemoveCurrentPacketHandler() {
    if (m_pCurrentPacketHandler) {
        delete m_pCurrentPacketHandler;
    }
}
