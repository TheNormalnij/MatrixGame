// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <list>
#include "commands/IGameCommand.h"
#include "net/transports/IServerTransport.h"
#include "net/sessions/CSessionStore.h"
#include "net/packets/IPacket.h"
#include <shared/game/Game.h>

class CGameNetwork {
public:
    CGameNetwork() = default;
    ~CGameNetwork() = default;

    void SendTickCommands(size_t tick, std::list<IGameCommand *> &commands);
    void SendGameStatusChanged(EGameStatus status);
    void SendGameInfo(std::string_view mapName);

private:
    void Broadcast(IPacket &packet);
    void Unicast(ISession* session, IPacket &packet);

private:
    CSessionStore *m_sessionStore;
};