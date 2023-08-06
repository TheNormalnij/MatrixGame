// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CCommandLog.h"

void CCommandLog::PushCommand(size_t tick, IGameCommand *command) {
    std::list<IGameCommand *> targetList = m_commandsByTick.at(tick);
    targetList.push_back(command);
}

std::list<IGameCommand *> *CCommandLog::GetTickCommands(size_t tick) {
    if (m_commandsByTick.size() <= tick) {
        return &m_commandsByTick[tick];
    }
    return nullptr;
}