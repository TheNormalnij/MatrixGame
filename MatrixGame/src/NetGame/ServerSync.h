// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <shared/game/CCommandLog.h>

class CServerSync {
public:
    CServerSync();
    ~CServerSync();

    bool NextTick();

    auto &GetCommands() { return m_commandLog.GetTickCommands(m_currentTick); };

    void OnGetServerCommands();
    void OnGetServerInfo();

private:
    size_t GetLastAllowerTick() const noexcept { return m_lastAllowedTick; };

private:
    size_t m_currentTick;
    size_t m_lastAllowedTick;
    CCommandLog m_commandLog;
};