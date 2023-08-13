// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <shared/game/CCommandLog.h>
#include "Handlers/INetGameHandler.h"

class CServerSync : public INetGameHandler {
public:
    CServerSync();
    ~CServerSync() = default;

    bool NextTick();

    bool IsGameInfoReady() const noexcept { return !m_mapName.empty(); };
    std::string_view GetMapName() const noexcept { return m_mapName; };
    uint32_t GetGameSeed() const noexcept { return m_seed; };

    EGameStatus GetGameStatus() const noexcept { return m_currentGameStatus; };

    auto GetCommands() { return m_commandLog.GetTickCommands(m_currentTick); };

    // Inherited via INetGameHanlder
    virtual void OnGetGameInfo(std::string_view mapName, uint32_t seed) override;
    virtual void OnChangeGameState(EGameStatus status) override { m_currentGameStatus = status; };

private:
    size_t GetLastAllowerTick() const noexcept { return m_lastAllowedTick; };

private:
    EGameStatus m_currentGameStatus;
    size_t m_currentTick;
    size_t m_lastAllowedTick;
    CCommandLog m_commandLog;

    std::string m_mapName;
    uint32_t m_seed;
};