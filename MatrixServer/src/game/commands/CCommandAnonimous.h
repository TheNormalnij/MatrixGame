// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "IGameCommand.h"

class CCommandAnonimous : public IGameCommand {
public:
    CCommandAnonimous(char *data, size_t len);

private:
    char *m_data;
    size_t m_len;
};