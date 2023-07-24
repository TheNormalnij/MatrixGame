// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "IServerTransport.h"
#include "ITransportHandler.h"
#include "../sessions/CSessionStore.h"
#include "uv.h"

class CServerTCPTransport : public IServerTransport {
public:
    CServerTCPTransport(uv_loop_t *loop, CSessionStore *sessionStore, ITransportHandler *handler);
    ~CServerTCPTransport() override;

    bool Listen(std::string_view host, uint16_t port) override;
    void Close(tranport_close_cb cb) override;
    void SendData(ISession *session, char *data, size_t len) override;

private:
    uv_loop_t *GetLoop() const { return m_loop; };
    ITransportHandler *GetHandler() const { return m_transportHandler; };

    static void StaticCloseCallback(uv_handle_t *server);
    static void StaticOnConnection(uv_stream_t *server, int status);
    static void StaticOnAlloc(uv_handle_t *client, size_t suggested_size, uv_buf_t *buf);
    static void StaticOnRecieve(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);

private:
    uv_loop_t *m_loop;
    uv_tcp_t m_server;
    CSessionStore *m_sessionStore;
    ITransportHandler* m_transportHandler;
    tranport_close_cb m_pendingCloseCb;
};
