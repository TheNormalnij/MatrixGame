// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CServerSync.h"
#include <list>

CServerSync::CServerSync() {
    m_currentGameStatus = EGameStatus::WAIT_PLAYERS;
    m_currentTick = 0;
    m_lastAllowedTick = 0;
    m_seed = 0;
}

bool CServerSync::NextTick() {
    if (m_currentTick >= GetLastAllowedTick()) {
        return false;
    }

    m_currentTick++;

    return true;
}

void CServerSync::OnGetGameInfo(std::string_view mapName, uint32_t seed) {
    m_mapName = mapName;
    m_seed = seed;
}

void CServerSync::OnGetTickCommands(size_t tick, std::vector<IGameCommand *> &commands) {
    m_commandLog.CopyTickCommands(tick, commands);

    TryAppendLastAllowedTick();
}

void CServerSync::TryAppendLastAllowedTick() {
    while (true) {
        if (!m_commandLog.HasTickCommands(m_lastAllowedTick + 1)) {
            return;
        }
        m_lastAllowedTick++;
    }
}

