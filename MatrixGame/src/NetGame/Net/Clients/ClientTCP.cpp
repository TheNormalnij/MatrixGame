// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "ClientTCP.h"
#include <format>
#include "../Utilits/AddressUtils.h"
#include <shared/defaults.h>

CClientTCP::CClientTCP() {
    m_hSocket = INVALID_SOCKET;
    m_pPacketHandler = nullptr;
}

CClientTCP::~CClientTCP() {

}

bool CClientTCP::Connect(std::string_view address) {
    const unsigned short port = AddressUtils::GetPortFromAddress(address, DEFAULT_NET_GAME_PORT);
    if (!port) {
        return false;
    }

    if (m_hSocket != INVALID_SOCKET) {
        return false;
    }

    if (!EnableNetwork()) {
        return false;
    }

    const int netType = GetAdressType(address);
    if (!CreateSocket(netType)) {
        return false;
    }

    // Connect to the remote server
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(address.data());
    serverAddr.sin_port = htons(port);

    auto iResult = connect(m_hSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (iResult == SOCKET_ERROR) {
        if (WSAGetLastError() != WSAEWOULDBLOCK) {
            printf("connect failed: %d\n", WSAGetLastError());
            closesocket(m_hSocket);
            WSACleanup();

            return false;
        }
    }

    // check socket is writable
    fd_set writeSet;
    FD_ZERO(&writeSet);
    FD_SET(m_hSocket, &writeSet);

    timeval timeout;
    timeout.tv_sec = 30;
    timeout.tv_usec = 0;

    iResult = select(0, nullptr, &writeSet, nullptr, &timeout);

    if (iResult == SOCKET_ERROR) {
        printf("connect failed: %d\n", WSAGetLastError());
        closesocket(m_hSocket);
        return false;
    }
    else if (iResult == 0) {
        printf("connect timed out\n");
        closesocket(m_hSocket);
        return false;
    }

    return true;
}

bool CClientTCP::Close() {
    int status = closesocket(m_hSocket);
    m_hSocket = INVALID_SOCKET;
    if (status == SOCKET_ERROR) {
        // const int errorCode = WSAGetLastError();
        // std::string error = std::format("Error code: {}", errorCode);
        return false;
    }
    else {
        return true;
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

bool CClientTCP::EnableNetwork() {
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("[TCP Client] WSAStartup failed: %d\n", iResult);
        return false;
    }
    return true;
}

bool CClientTCP::CreateSocket(int netType) {
    m_hSocket = WSASocketW(netType, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (m_hSocket == INVALID_SOCKET) {
        printf("[TCP Client] CreateSocket socket failed: %d\n", WSAGetLastError());
        WSACleanup();
        return false;
    }

    // Set the socket to nonblocking mode
    u_long iMode = 1;
    auto iResult = ioctlsocket(m_hSocket, FIONBIO, &iMode);
    if (iResult != NO_ERROR) {
        printf("ioctlsocket failed: %ld\n", iResult);
        closesocket(m_hSocket);
        WSACleanup();
        return false;
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
