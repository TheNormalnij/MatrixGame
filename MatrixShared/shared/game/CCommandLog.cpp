// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CCommandLog.h"

constexpr const size_t ADDENUM_SIZE = 10000;

void CCommandLog::PushCommand(size_t tick, IGameCommand *command) {
    std::list<IGameCommand *> *targetList = m_commandsByTick.at(tick);
    if (!targetList) {
        targetList = new std::list<IGameCommand *>();
        ExtendLogTo(tick);
        m_commandsByTick[tick] = targetList;
    }
    targetList->push_back(command);
}

std::list<IGameCommand *> *CCommandLog::GetTickCommands(size_t tick) {
    if (m_commandsByTick.size() >= tick) {
        return m_commandsByTick[tick];
    }
    return nullptr;
}

void CCommandLog::SetTickCommands(size_t tick, std::list<IGameCommand*> *commads) {
    ExtendLogTo(tick);
    if (m_commandsByTick[tick]) {
        delete m_commandsByTick[tick];
    }
    m_commandsByTick[tick] = commads;
}

void CCommandLog::CopyTickCommands(size_t tick, std::vector<IGameCommand*> &commads) {
    auto targetList = new std::list<IGameCommand *>();

    for (IGameCommand *cmd : commads) {
        targetList->push_back(cmd);
    }

    SetTickCommands(tick, targetList);
}

bool CCommandLog::HasTickCommands(size_t tick) const noexcept {
    if (m_commandsByTick.size() < tick) {
        return false;
    }

    return m_commandsByTick[tick] != nullptr;
}

void CCommandLog::ExtendLogTo(size_t tick) {
    if (m_commandsByTick.size() >= tick) {
        return;
    }

    m_commandsByTick.resize(tick + ADDENUM_SIZE);
}
