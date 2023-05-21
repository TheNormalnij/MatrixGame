// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once
#include "../MatrixGameDll.hpp"
#include "RangersSound.h"
#include "RangersText.h"
#include "RangersGame.h"

class RangersInterfaceInternal {
    RangersInterfaceInternal();
    ~RangersInterfaceInternal() = default;

public:
    static RangersInterfaceInternal* getInstance();

    SMGDRangersInterface *getSMGDIntervace() { return &m_sMGDRobotInterface; };

    RangersSound *GetSound() { return &m_soundInterface; };
    RangersText *GetText() { return &m_textInterface; };
    RangersGame *GetGame() { return &m_rangersGameInterface; };

private:
    void LinkInterface();

private:
    RangersSound m_soundInterface;
    RangersText m_textInterface;
    RangersGame m_rangersGameInterface;

    SMGDRangersInterface m_sMGDRobotInterface;
};