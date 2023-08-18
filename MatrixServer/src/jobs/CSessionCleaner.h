// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "IServerJob.h"
#include "net/sessions/CSessionStore.h"

class CSessionCleaner : public IServerJob {
public:
    CSessionCleaner(CSessionStore *store) : m_sessionStore(store){};
    ~CSessionCleaner() = default;
    void Update() override;

private:
    CSessionStore *m_sessionStore;
};