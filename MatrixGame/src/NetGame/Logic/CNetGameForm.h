// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "Form.hpp"
#include "MatrixKeyScan.hpp"
#include "INetGameLogic.h"
#include "Control/COrderController.h"

#include <deque>
#include <string>

class CNetGameForm : public CForm {
public:
    CNetGameForm(INetGameLogic *logic, COrderController *orderController);
    ~CNetGameForm();

    // CForm interface
    void Enter() override;
    void Leave() override;
    void Draw() override;

    void Takt(int step) override;

    void MouseMove(int x, int y) override;
    void MouseKey(ButtonStatus status, int key, int x, int y) override;

    void Keyboard(bool down, int scan) override;
    void SystemEvent(ESysEvent se) override;
    void MinimapClick(int key);

private:
    bool IsRobotManualControllEnabled() const;
    void MoveCameraByScreenBorders();
    void MoveCameraByKeyboard();
    void RotateCameraByKeyboard();
    bool ApplyZoomCameraByWheel(ButtonStatus status, int pos);
    bool ApplyMouseCamStatus(ButtonStatus status, int key);
    void CheckSelectionUp(ButtonStatus status, int key);
    void TryStartSelection();

    void DelegateClickToInterface(ButtonStatus status, int key);
    void DelegateClickToMap(ButtonStatus status, int key, int x, int y);

    bool IsInputEqual(std::string str);
private:
    COrderController *m_pOrderController;
    INetGameLogic *m_logic;
    bool m_wasReadyClicked;

    float m_LastWorldX, m_LastWorldY;
    int m_Action;

    std::deque<SKeyScan> m_LastScans{MAX_SCANS};
    
};
