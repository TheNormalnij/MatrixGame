// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "IServerTransport.h"
#include "uv.h"

class CServerTCPTransport : public IServerTransport {
public:
    CServerTCPTransport(uv_loop_t *loop);

private:
    ~CServerTCPTransport();

public:
    bool Listen(std::string_view host, uint16_t port) override;
    void Close() override;
    void SetPacketHandler(packet_handler handler) override;
    void SendPacket(const struct sockaddr *addr, char *data, uint16_t count);
    void Release();

private:
    void OnConnection(uv_stream_t *server, int status);

private:
    uv_loop_t *m_loop;
    uv_tcp_t m_server;
    packet_handler m_currentPacketHandlerFun;
};
