// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "NetGame.h"
#include "NetGameForm.h"

bool CNetGame::ConnectGame(std::string_view host) {
    return false;
}

void CNetGame::StartNetworkGame(wchar *map, uint32_t seed) {
    SMatrixTextParams textReplace = {0};
    m_currentGame.Init(m_hAppInstance, NULL, map, seed, m_pMatrixSettings, &textReplace);

    CNetGameForm formgame;

    m_currentGame.RunGameLoop(&formgame);

    //m_currentGame.SaveResult(rgs);
    m_currentGame.SafeFree();

}

void CNetGame::StopNetworkGame() {

}
