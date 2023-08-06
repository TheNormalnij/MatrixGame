// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "ClientTCP.h"
#include <format>
#include <ws2tcpip.h>

CClientTCP::CClientTCP() {
    m_hSocket = INVALID_SOCKET;
    m_pPacketHandler = nullptr;
}

CClientTCP::~CClientTCP() {

}

void CClientTCP::Connect(std::string_view adress, net_client_connect_cb callback) {
    if (m_hSocket == INVALID_SOCKET) {
        const int netType = GetAdressType(adress);
        CreateSocket(netType);
    }

    // Connect to the remote server
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(adress.data());
    serverAddr.sin_port = htons(1234);

    auto iResult = connect(m_hSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (iResult == SOCKET_ERROR) {
        if (WSAGetLastError() != WSAEWOULDBLOCK) {
            printf("connect failed: %d\n", WSAGetLastError());
            closesocket(m_hSocket);
            WSACleanup();
            return;
        }
    }

    callback(this, true, "");
}

void CClientTCP::Close(net_client_close_cb callback) {
    int status = closesocket(m_hSocket);
    m_hSocket = INVALID_SOCKET;
    if (status == SOCKET_ERROR) {
        const int errorCode = WSAGetLastError();
        std::string error = std::format("Error code: {}", errorCode);
        callback(this, false, error);
    }
    else {
        callback(this, true, "");
    }
}

void CClientTCP::SendData(CRequest *req) {
    int bytesSend = send(m_hSocket, req->GetData(), req->GetSize(), 0);
    if (bytesSend < 0) {
        std::printf("[TCP Client] SendData error: %d", bytesSend);
    }
}

void CClientTCP::DoUpdate() {
    // Receive data from the socket
    char buffer[1024];
    int bytesReceived = 0;

    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(m_hSocket, &readSet);

    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    int iResult = select(0, &readSet, NULL, NULL, &timeout);

    if (iResult == SOCKET_ERROR) {
        printf("select failed: %d\n", WSAGetLastError());
        return;
    }
    else if (iResult == 0) {
        printf("select timed out\n");
        return;
    }

    bytesReceived = recv(m_hSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived == SOCKET_ERROR) {
        if (WSAGetLastError() == WSAEWOULDBLOCK) {
            return;
        }
        else {
            printf("recv failed: %d\n", WSAGetLastError());
            return;
        }
    }
    else if (bytesReceived == 0) {
        printf("connection closed by peer\n");
        return;
    }

    // Do something with the received data
    printf("received %d bytes: %.*s\n", bytesReceived, bytesReceived, buffer);

    CReadStream stream = CReadStream(buffer, bytesReceived);

    m_pPacketHandler->Handle(&stream);
}

bool CClientTCP::CreateSocket(int netType) {
    m_hSocket = WSASocketW(netType, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (m_hSocket == INVALID_SOCKET) {
        printf("[TCP Client] CreateSocket socket failed: %d\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    // Set the socket to nonblocking mode
    u_long iMode = 1;
    auto iResult = ioctlsocket(m_hSocket, FIONBIO, &iMode);
    if (iResult != NO_ERROR) {
        printf("ioctlsocket failed: %ld\n", iResult);
        closesocket(m_hSocket);
        WSACleanup();
        return;
    }

    return true;
}

// TODO: looks like a utility function
int CClientTCP::GetAdressType(std::string_view adress) const noexcept {
    // Very laszy IPv4 / IPv6 selector
    if (adress.find(".")) {
        return AF_INET;
    }
    else {
        return AF_INET6;
    }
}
