// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "IPlayer.h"

class CPlayerBase : public IPlayer {
public:
    void SetReady(bool status) override { m_status = status; };
    bool IsReady() const noexcept override { return m_status; };
    ISession *GetSession() const noexcept override { return nullptr; };

private:
    bool m_status = false;
};
