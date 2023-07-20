// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CSessionCleaner.h"
#include <ctime>

constexpr uint64_t SESSION_DROP_TIME = 30;

void CSessionCleaner::DoTick() {
    const uint64_t now = std::time(nullptr);
    for (ISession *session : m_sessionStore->GetSessions()) {
        if (now - session->GetLastUpdate() > SESSION_DROP_TIME) {
            m_sessionStore->RemoveSession(session);
        }
    }
}
