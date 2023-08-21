// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <list>
#include <shared/game/IGameCommand.h>
#include "net/transports/IServerTransport.h"
#include "net/sessions/CSessionStore.h"
#include <shared/net/CRequest.h>
#include <shared/game/Game.h>
#include <shared/net/IPacket.h>

class CGameNetwork {
public:
    CGameNetwork(CSessionStore *sessionStore) : m_sessionStore(sessionStore){};
    ~CGameNetwork() = default;

    void SendTickCommands(size_t tick, std::list<IGameCommand *> *commands);
    void SendGameStatusChanged(EGameStatus status);
    void SendGameInfo(ISession *session, std::string_view mapName);
    void SendConnect(ISession *session);

private:
    void Broadcast(IPacket &packet);
    void Unicast(ISession* session, IPacket &packet);

private:
    CSessionStore *m_sessionStore;
};