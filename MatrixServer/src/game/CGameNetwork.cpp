// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CGameNetwork.h"


void CGameNetwork::SendTickCommands(size_t tick, std::list<IGameCommand *> commands) {

}

void CGameNetwork::Broadcast(char* data, size_t len) {
    for (ISession *session : m_sessionStore->GetSessions()) {

    }
}
