// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <list>
#include <string_view>
#include "ISession.h"

class CSessionStore {
public:
    CSessionStore() = default;
    ~CSessionStore() = default;

    bool AddSession(ISession *session);
    bool RemoveSession(ISession *session);

    bool IsSessionActive(ISession* session);

    ISession *GetSessionByToken(std::string_view sessionToken);

    std::list<ISession *> &GetSessions() { return m_sessions; };

private:
    std::list<ISession *> m_sessions;
};
