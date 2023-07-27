// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once
#include <string_view>
#include "../transports/CRequest.h"

class ISession {
public:
    virtual ~ISession() = default;
  
    virtual std::string_view GetToken() = 0;

    // Send data
    virtual void SendData(CRequest *req) = 0;

    // Handler should set timestamp here to mark session active
    virtual void SetLastClientUpdateTimestamp(uint64_t timestamp) = 0;

    // returns unix timestamp ms
    virtual uint64_t GetLastClientUpdateTimestamp() = 0;

    // Set custom data to session
    virtual void SetCustomData(void *data) = 0;

    // Get custom data from session
    virtual void *GetCustomData() = 0;
};
