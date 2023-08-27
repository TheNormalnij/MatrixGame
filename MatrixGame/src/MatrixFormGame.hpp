// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "Form.hpp"
#include "MatrixKeyScan.hpp"
#include "Control/COrderController.h"
#include "Control/CLocalOrderProcessor.h"

#include <deque>
#include <string>

class CFormMatrixGame : public CForm {
private:
    float m_LastWorldX, m_LastWorldY;
    int m_Action;

    std::deque<SKeyScan> m_LastScans{MAX_SCANS};
    bool IsInputEqual(std::string str);

public:
    CFormMatrixGame(void);
    ~CFormMatrixGame();

    virtual void Enter(void);
    virtual void Leave(void);
    virtual void Draw(void);

    virtual void Takt(int step);

    virtual void MouseMove(int x, int y);
    virtual void MouseKey(ButtonStatus status, int key, int x, int y);

    virtual void Keyboard(bool down, int scan);
    virtual void SystemEvent(ESysEvent se);

    void MinimapClick(int key);
    COrderController *GetOrderController() const noexcept override { return m_pOrderController; };

private:
    CLocalOrderProcessor *m_pOrderProcessor;
    COrderController *m_pOrderController;
};
