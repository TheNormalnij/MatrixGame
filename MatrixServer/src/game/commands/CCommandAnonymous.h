// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "IGameCommand.h"

class CCommandAnonymous : public IGameCommand {
public:
    CCommandAnonymous(char *data, size_t len);
    ~CCommandAnonymous() override;

    void Write(CWriteStream &stream) override;
    void Read(CReadStream &stream) override;

private:
    char *m_data;
    size_t m_len;
};