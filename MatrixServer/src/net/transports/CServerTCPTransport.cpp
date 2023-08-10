// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CServerTCPTransport.h"
#include "../sessions/CSessionTCP.h"

constexpr const unsigned short BUFFER_SIZE = 1380;
constexpr const int MAX_CONNECTIONS = 100;

CServerTCPTransport::CServerTCPTransport(uv_loop_t *loop, CSessionStore *sessionStore, ITransportHandler *handler) {
    m_loop = loop;
    m_server = {0};
    m_transportHandler = handler;
    m_sessionStore = sessionStore;
    m_pendingCloseCb = nullptr;
}

CServerTCPTransport::~CServerTCPTransport() {

}

bool CServerTCPTransport::Listen(std::string_view host, uint16_t port) {
    int status;
    struct sockaddr_in addr;
    uv_loop_t *loop = uv_default_loop();

    printf("Starting server on port: %d...\n", port);

    status = uv_tcp_init(loop, &m_server);
    if (status < 0) {
        printf("Can not start main loop. Error code: %d\n", status);
        return false;
    }

    m_server.data = this;

    uv_ip4_addr(host.data(), port, &addr);

    status = uv_tcp_bind(&m_server, (const struct sockaddr *)&addr, 0);
    if (status < 0) {
        printf("Can not bind server. Address used? Error code: %d\n", status);
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

void CServerTCPTransport::SendData(ISession *session, CRequest *req) {
    uv_write_t *uvreq = new uv_write_t();

    const uv_buf_t wrbuf = uv_buf_init(req->GetData(), req->GetSize());
    uvreq->data = req;

    const uv_tcp_t *handler = (uv_tcp_t*)session->GetHandler();

    uv_write(uvreq, (uv_stream_t *)handler, &wrbuf, 1, [](uv_write_t *uvreq, int status) {
        // Write callback
        if (status) {
            fprintf(stderr, "[TCP] Write error %s\n", uv_strerror(status));
        }

        CRequest *req = (CRequest *)uvreq->data;
        req->CallCallback(status == 0);

        delete uvreq;
    });
}

void CServerTCPTransport::Close(tranport_close_cb cb) {
    if (m_pendingCloseCb) {
        return;
    }

    m_pendingCloseCb = cb;

    uv_close((uv_handle_t *)&m_server, CServerTCPTransport::StaticCloseCallback);
}

void CServerTCPTransport::StaticCloseCallback(uv_handle_t * server)
{
    CServerTCPTransport *self = (CServerTCPTransport *)server->data;
    self->m_pendingCloseCb(self);
}

void CServerTCPTransport::StaticOnConnection(uv_stream_t *server, int status) {
    if (status < 0) {
        fprintf(stderr, "New connection error %s\n", uv_strerror(status));
        return;
    }

    CServerTCPTransport* self = (CServerTCPTransport*) server->data;

    CSessionTCP *client = new CSessionTCP(self->GetHandler(), self);
    client->RegisterInEventLoop(self->GetLoop());

    if (uv_accept(server, (uv_stream_t *)client->GetHandler()) == 0) {
        uv_read_start((uv_stream_t *)client->GetHandler(), CServerTCPTransport::StaticOnAlloc,
                      CServerTCPTransport::StaticOnRecieve);

        self->m_sessionStore->AddSession(client);
    } else {
        client->CloseAndDestroy();
    }
}
 
void CServerTCPTransport::StaticOnAlloc(uv_handle_t *client, size_t suggested_size, uv_buf_t *buf) {
    // Suggested_size is always 64KB for localhost
    buf->base = new char[suggested_size];
    buf->len = suggested_size;
}

void CServerTCPTransport::StaticOnRecieve(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
    if (nread > 0) {
        CSessionTCP *session = (CSessionTCP *)client->data;
        session->GetTransportHandler()->HandlePacket(session, buf->base, buf->len);
    }
    else if (nread < 0) {
        // -4077 is connection close
        printf( "Connection error: %s\n", uv_strerror(nread));
        uv_close((uv_handle_t *)client, nullptr);
    }
    
    if (buf->base != nullptr) {
        delete[] buf->base;   
    }
}