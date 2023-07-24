// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "ISession.h"
#include <cstdint>

class CSessionBase : public ISession {
    // Transport should set timestamp here to mark session active
    void SetLastClientUpdateTimestamp(uint64_t timestamp) override { m_lastClientUpdate = timestamp; };

    // returns unix timestamp ms
    uint64_t GetLastClientUpdateTimestamp() override { return m_lastClientUpdate; };

    // Set custom data to session
    void SetCustomData(void *data) override { m_data = data; };

    // Get custom data from session
    void *GetCustomData() override { return m_data; };

private:
    uint64_t m_lastClientUpdate = 0;
    void *m_data = nullptr;
};