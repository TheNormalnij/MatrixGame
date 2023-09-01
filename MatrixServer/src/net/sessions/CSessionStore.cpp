// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CSessionStore.h"

bool CSessionStore::AddSession(ISession *session) {
    if (IsSessionActive(session))
        return false;

    m_sessions.push_back(session);
    return true;
}

bool CSessionStore::RemoveSession(ISession *session) {
    return m_sessions.remove(session) > 0;
}

bool CSessionStore::IsSessionActive(ISession *session) {
    for (const ISession* anotherSession : m_sessions) {
        if (session == anotherSession) {
            return true;
        }
    }
    return false;
}

ISession *CSessionStore::GetSessionByToken(std::string_view sessionToken) {
    for (ISession *session : m_sessions) {
        if (session->GetToken().compare(sessionToken) == 0) {
            return session;
        }
    }
    return nullptr;
}
