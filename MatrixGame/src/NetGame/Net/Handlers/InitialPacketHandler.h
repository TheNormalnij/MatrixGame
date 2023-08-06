// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "IPacketHandler.h"
#include <functional>
#include <string_view>

typedef std::function<void(bool succees, const char* error)> server_initial_cb;

// This packet handler should prevent connecting to wrong server

class CInitialPacketHandler : public IPacketHandler {
public:
    CInitialPacketHandler(server_initial_cb cb) : m_callback(cb){};

    // IPacketHandler interface
    virtual void Handle(CReadStream *stream) override;

private:
    server_initial_cb m_callback;
};