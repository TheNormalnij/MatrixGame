// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "IGameCommand.h"
#include <list>
#include <vector>

class CCommandLog {
public:
    CCommandLog() = default;
    ~CCommandLog() = default;

    void PushCommand(size_t tick, IGameCommand *commad);
    std::list<IGameCommand*> *GetTickCommands(size_t tick);
    
    void SetTickCommands(size_t tick, std::list<IGameCommand *> *commads);

    // TODO: template ?
    void CopyTickCommands(size_t tick, std::vector<IGameCommand *> &commads);
    bool HasTickCommands(size_t tick) const noexcept;

private:
    void ExtendLogTo(size_t tick);

private:
    std::vector<std::list<IGameCommand *>*> m_commandsByTick;
};
