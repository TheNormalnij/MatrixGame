// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "IPlayer.h"
#include "../../net/sessions/ISession.h"

class CNetPlayer : public IPlayer {
public:
    CNetPlayer(ISession session) : m_session(session){};
    ~CNetPlayer() = default;

    ISession *GetSession() { return m_session; };
   
private:
    ISession *m_session;
};
