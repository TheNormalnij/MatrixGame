// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <shared/net/CReadStream.h>
#include "ITransportDataHandler.h"
#include "INetGameHandler.h"

class CGameNetDataHandler : public ITransportDataHandler {
public:
    CGameNetDataHandler(INetGameHandler *pGame) : m_pGame(pGame){};
    ~CGameNetDataHandler() override = default;

    void Handle(CReadStream *stream) override;

private:
    INetGameHandler *m_pGame;
};