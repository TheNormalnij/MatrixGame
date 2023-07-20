// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CServerTCPTransport.h"

const int MAX_CONNECTIONS = 100;

static void on_recv(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
    if (nread > 0) {
        printf("%lu\n", nread);
        printf("%s", buf->base);
    }
    printf("free  :%lu %p\n", buf->len, buf->base);

    delete[] buf->base;
}

static void on_alloc(uv_handle_t *client, size_t suggested_size, uv_buf_t *buf) {
    buf->base = new char[suggested_size];
    buf->len = suggested_size;
    printf("malloc:%lu %p\n", buf->len, buf->base);
}

static void on_new_connection(uv_stream_t *server, int status) {
    CServerTCPHandler *self = (CServerTCPHandler*) server->data;
    self->OnConnection(server, status);
}

CServerTCPTransport::CServerTCPTransport(uv_loop_t *loop) {
    m_loop = loop;
    m_server = {0};
}

CServerTCPTransport::~CServerTCPTransport() {}

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

    int r = uv_listen((uv_stream_t *)&m_server, MAX_CONNECTIONS, on_new_connection);
    if (r) {
        fprintf(stderr, "Can not start TCP server: %s\n", uv_strerror(r));
        return false;
    }

    return true;
}

void CServerTCPTransport::OnConnection(uv_stream_t *server, int status) {
    if (status < 0) {
        fprintf(stderr, "New connection error %s\n", uv_strerror(status));
        return;
    }

    uv_tcp_t *client = (uv_tcp_t *)malloc(sizeof(uv_tcp_t));
    uv_tcp_init(m_loop, client);
    if (uv_accept(server, (uv_stream_t *)client) == 0) {
        uv_read_start((uv_stream_t *)client, on_alloc, on_recv);
    }
}

void CServerTCPTransport::SetPacketHandler(packet_handler handler) {
    m_currentPacketHandlerFun = handler;
}

void send_cb(uv_udp_send_t *req, int status) {
    delete req;
}

void CServerTCPTransport::SendPacket(const struct sockaddr *addr, char *data, uint16_t count) {
    uv_udp_send_t *req = new uv_udp_send_t();

    uv_buf_t buf;
    buf.base = data;
    buf.len = count;

    //uv_tcp_send(req, &m_server, &buf, count, addr, send_cb);
}

void CServerTCPTransport::Close() {
    uv_close((uv_handle_t *)&m_server, nullptr);
}

void CServerTCPTransport::Release() {
    delete this;
}