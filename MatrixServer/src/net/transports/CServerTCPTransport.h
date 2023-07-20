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
    ~CServerTCPTransport() override;

    bool Listen(std::string_view host, uint16_t port) override;
    void Close(tranport_close_cb cb) override;
    void SetPacketHandler(packet_handler handler) override;
    void SendPacket(void* handler, char *data, uint16_t count);
private:
    static void StaticOnConnection(uv_stream_t *server, int status);
    static void StaticOnAlloc(uv_handle_t *client, size_t suggested_size, uv_buf_t *buf);
    static void StaticOnRecieve(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);

private:
    uv_loop_t *m_loop;
    uv_tcp_t m_server;
    packet_handler m_currentPacketHandlerFun;
};
