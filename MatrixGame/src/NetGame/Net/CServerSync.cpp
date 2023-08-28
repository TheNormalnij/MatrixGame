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
    m_side = -1;
    m_sideAiStatus = 0xFF;
}

bool CServerSync::NextTick() {
    if (m_currentTick >= GetLastAllowedTick()) {
        return false;
    }

    m_currentTick++;

    return true;
}

void CServerSync::OnGetGameInfo(std::string_view mapName, uint32_t seed, uint8_t side, uint8_t sideAiStatus) {
    m_mapName = mapName;
    m_seed = seed;
    m_side = side;
    m_sideAiStatus = sideAiStatus;
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

