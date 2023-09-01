// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "CPlayerBase.h"

class CNetPlayer : public CPlayerBase {
public:
    CNetPlayer(ISession *session) : m_session(session){};
    ~CNetPlayer() = default;

    ISession *GetSession() const noexcept { return m_session; };
   
private:
    ISession *m_session;
};
