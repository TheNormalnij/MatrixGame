// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "MatrixGameDllEx.hpp"
#include "MatrixGame.h"
#include <string_view>

class CNetGame {
public:
    CNetGame(HINSTANCE hInstance, SMatrixSettings *set) : m_hAppInstance(hInstance), m_pMatrixSettings(set) {};
    ~CNetGame() = default;

    bool ConnectGame(std::string_view host);

private:
    void StartNetworkGame(wchar *map, uint32_t seed);
    void StopNetworkGame();

private:
    HINSTANCE m_hAppInstance;
    SMatrixSettings *m_pMatrixSettings;
    CGame m_currentGame;
};