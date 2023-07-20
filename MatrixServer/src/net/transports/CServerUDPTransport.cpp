// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CServerUDPTransport.h"

static void on_recv(uv_udp_t *handle, ssize_t nread, const uv_buf_t *rcvbuf, const struct sockaddr *addr,
                    unsigned flags) {
    if (nread > 0) {
        printf("%lu\n", nread);
        printf("%s", rcvbuf->base);
    }
    printf("free  :%lu %p\n", rcvbuf->len, rcvbuf->base);

    delete[] rcvbuf->base;
}

static void on_alloc(uv_handle_t *client, size_t suggested_size, uv_buf_t *buf) {
    buf->base = new char[suggested_size];
    buf->len = suggested_size;
    printf("malloc:%lu %p\n", buf->len, buf->base);
}

CServerUDPTransport::CServerUDPTransport(uv_loop_t *loop) {
    m_loop = loop;
    m_server = {0};
}

CServerUDPTransport::~CServerUDPTransport() {}

bool CServerUDPTransport::Listen(std::string_view host, uint16_t port) {
    int status;
    struct sockaddr_in addr;
    uv_loop_t *loop = uv_default_loop();

    printf("Starting server on port: %d...", port);

    status = uv_udp_init(loop, &m_server);
    if (status < 0) {
        printf("Can not start main loop. Error code: %d", status);
        return false;
    }

    m_server.data = this;

    uv_ip4_addr(host.data(), port, &addr);

    status = uv_udp_bind(&m_server, (const struct sockaddr *)&addr, 0);
    if (status < 0) {
        printf("Can not bind server. Address used? Error code: %d", status);
        return false;
    }

    status = uv_udp_recv_start(&m_server, on_alloc, on_recv);
    if (status < 0) {
        printf("Can not start UDP server. Error code: %d", status);
        return false;
    }

    return true;
}

void CServerUDPTransport::SetPacketHandler(cb_packet_handler handler) {

}

void send_cb(uv_udp_send_t* req, int status) {
    delete req;
}

void CServerUDPTransport::SendPacket(const struct sockaddr *addr, char *data, uint16_t count) {
    uv_udp_send_t *req = new uv_udp_send_t();

    uv_buf_t buf;
    buf.base = data;
    buf.len = count;

    uv_udp_send(req, &m_server, &buf, count, addr, send_cb);
}

void CServerUDPTransport::Close() {
    uv_close((uv_handle_t *)&m_server, nullptr);
}

void CServerUDPTransport::Release() {
    delete this;
}