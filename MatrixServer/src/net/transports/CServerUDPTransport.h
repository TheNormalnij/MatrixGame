// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "IServerTransport.h"
#include "uv.h"

typedef void (*cb_packet_handler)(void *handle, size_t nread, const char *buf);

class CServerUDPTransport : public IServerTransport {
public:
    CServerUDPTransport(uv_loop_t *loop);

private:
    ~CServerUDPTransport();

public:
    bool Listen(std::string_view host, uint16_t port) override;
    void Close() override;
    void SetPacketHandler(cb_packet_handler handler);
    void SendPacket(const struct sockaddr *addr, char *data, uint16_t count);
    void Release();

private:
    uv_loop_t *m_loop;
    uv_udp_t m_server;
};
