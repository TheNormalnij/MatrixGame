// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "commands/IGameCommand.h"
#include <list>
#include <vector>

class CCommandLog {
public:
    CCommandLog() = default;
    ~CCommandLog() = default;

    void PushCommand(size_t tick, IGameCommand *commad);
    std::list<IGameCommand*> &GetTickCommands(size_t tick);

private:
    std::vector<std::list<IGameCommand *>> m_commandsByTick;
};
