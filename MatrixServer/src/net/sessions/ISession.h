// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once
#include <string_view>

class ISession {
public:
    virtual ~ISession() = 0;
    virtual bool IsSource(void* source) = 0;
    virtual std::string_view GetToken() = 0;
    virtual uint64_t GetLastUpdate() = 0;
    virtual void SendData(char *data, size_t size) = 0;
};
