// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "ServerConnection.h"
#include "Clients/CClientTCP.h"

CServerConnection::CServerConnection(std::string_view host) {
    m_host = host;
    m_status = EServerConnectionStatus::DISCONNECTED;
    m_pTransport = new CClientTCP();
}

void CServerConnection::Connect() {
    if (m_status != EServerConnectionStatus::DISCONNECTED) {
        return;
    }

    m_status == EServerConnectionStatus::CONNECTING;
    StartTransport();
}

void CServerConnection::StartTransport() {

     net_client_connect_cb cb = [](INetworkClient *client, bool success, std::string_view error) {
        if (success) {
            TryHandshake();
        }
        else {
            m_status = EServerConnectionStatus::DISCONNECTED;
        }
        m_status = EServerConnectionStatus::CONNECTED;
    };

    m_pTransport->Connect(m_host, cb);
}

void CServerConnection::TryHandshake() {

}
