// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <list>
#include "commands/IGameCommand.h"
#include "../net/transports/IServerTransport.h"
#include "../net/sessions/CSessionStore.h"

class CGameNetwork {
public:
    void SendTickCommands(size_t tick, std::list<IGameCommand *> commands);
    void SendEvent();
    void SendGameStart();
    void SendGameStop();

private:
    void Broadcast(char *data, size_t len);
    void Unicast(ISession* session, char *data, size_t len);

private:
    CSessionStore *m_sessionStore;
};