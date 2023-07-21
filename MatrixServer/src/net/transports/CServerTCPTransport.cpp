// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CServerTCPTransport.h"
#include "../sessions/CSessionTCP.h"

constexpr int MAX_CONNECTIONS = 100;

CServerTCPTransport::CServerTCPTransport(uv_loop_t *loop) {
    m_loop = loop;
    m_server = {0};
}

CServerTCPTransport::~CServerTCPTransport() {

}

bool CServerTCPTransport::Listen(std::string_view host, uint16_t port) {
    int status;
    struct sockaddr_in addr;
    uv_loop_t *loop = uv_default_loop();

    printf("Starting server on port: %d...", port);

    status = uv_tcp_init(loop, &m_server);
    if (status < 0) {
        printf("Can not start main loop. Error code: %d", status);
        return false;
    }

    m_server.data = this;

    uv_ip4_addr(host.data(), port, &addr);

    status = uv_tcp_bind(&m_server, (const struct sockaddr *)&addr, 0);
    if (status < 0) {
        printf("Can not bind server. Address used? Error code: %d", status);
        return false;
    }

    uv_tcp_nodelay(&m_server, true);

    int r = uv_listen((uv_stream_t *)&m_server, MAX_CONNECTIONS, CServerTCPTransport::StaticOnConnection);

    if (r) {
        fprintf(stderr, "Can not start TCP server: %s\n", uv_strerror(r));
        return false;
    }

    return true;
}

void CServerTCPTransport::SetPacketHandler(packet_handler handler) {
    m_currentPacketHandlerFun = handler;
}

void CServerTCPTransport::SendPacket(void *hander, char *data, uint16_t count) {
    uv_udp_send_t *req = new uv_udp_send_t();

    uv_buf_t wrbuf = uv_buf_init(data, count);

    uv_write(req, handler, &wrbuf, 1, [](uv_write_t *req, int status) {
        // Write callback
        if (status) {
            fprintf(stderr, "Write error %s\n", uv_strerror(status));
        }
        delete reg;
    });
}

void CServerTCPTransport::Close(tranport_close_cb cb) {
    uv_close((uv_handle_t *)&m_server, [&](void *handler) { cb(this); });
}

void CServerTCPTransport::StaticOnConnection(uv_stream_t *server, int status) {
    if (status < 0) {
        fprintf(stderr, "New connection error %s\n", uv_strerror(status));
        return;
    }

    CSessionTCP *client = new CSessionTCP();
    client->RegisterInEventLoop(m_loop);

    if (uv_accept(server, (uv_stream_t *)client->GetHandler()) == 0) {
        uv_read_start((uv_stream_t *)client->GetHandler(), CServerTCPTransport::StaticOnAlloc,
                      CServerTCPTransport::StaticOnRecieve);
    } else {
        client->CloseAndDestroy();
    }
}
 
void CServerTCPTransport::StaticOnAlloc(uv_handle_t *client, size_t suggested_size, uv_buf_t *buf) {
    buf->base = new char[suggested_size];
    buf->len = suggested_size;
    printf("[TCP transport] Allocate:%lu %p\n", buf->len, buf->base);
}

void CServerTCPTransport::StaticOnRecieve(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
    if (nread > 0) {
        printf("%lu\n", nread);
        printf("%s", buf->base);
    }
    printf("free  :%lu %p\n", buf->len, buf->base);

    delete[] buf->base;
}
