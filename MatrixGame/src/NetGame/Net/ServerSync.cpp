// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "ServerSync.h"

CServerSync::CServerSync() {
    m_currentGameStatus = EGameStatus::WAIT_PLAYERS;
}

bool CServerSync::NextTick() {
    if (m_currentTick >= GetLastAllowerTick()) {
        return false;
    }

    m_currentTick++;

    return true;
}

void CServerSync::OnGetGameInfo(std::string_view mapName, uint32_t seed) {
    m_mapName = mapName;
    m_seed = seed;
}

