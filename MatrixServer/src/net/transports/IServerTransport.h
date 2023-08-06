// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <string_view>
#include <stdint.h>
#include <shared/net/CRequest.h>
#include "../sessions/ISession.h"

class IServerTransport;

typedef void (*packet_handler)(void *source, size_t len, char *data);
typedef void (*tranport_close_cb)(IServerTransport *transport);

// Adapter interface for transport service TCP/UDP/Websockets...
class IServerTransport {
public:
    // Free resources
    virtual ~IServerTransport() = default;
    // Start server
    virtual bool Listen(std::string_view host, uint16_t port) = 0;
    // Stop server
    virtual void Close(tranport_close_cb cb) = 0;
    // Send data
    virtual void SendData(ISession *session, CRequest *req) = 0;
};