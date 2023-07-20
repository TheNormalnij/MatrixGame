// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <string_view>
#include <stdint.h>

typedef void (*packet_handler)(void *source, size_t len, char *data);

class IServerTransport {
public:
    // Start server
    virtual bool Listen(std::string_view host, uint16_t port) = 0;
    // Stop server
    virtual void Close() = 0;
    // Set data handler
    virtual void SetPacketHandler(packet_handler handler) = 0;
    // Release resources, free data
    virtual void Release() = 0;
};