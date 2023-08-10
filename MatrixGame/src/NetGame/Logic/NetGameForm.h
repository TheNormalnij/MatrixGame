// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "Form.hpp"
#include "MatrixKeyScan.hpp"

#include <deque>
#include <string>

class CNetGameForm : public CForm {
private:
    float m_LastWorldX, m_LastWorldY;
    int m_Action;

    std::deque<SKeyScan> m_LastScans{MAX_SCANS};
    bool IsInputEqual(std::string str);

public:
    CNetGameForm(void);
    ~CNetGameForm();

    virtual void Enter(void);
    virtual void Leave(void);
    virtual void Draw(void);

    virtual void Takt(int step);

    virtual void MouseMove(int x, int y);
    virtual void MouseKey(ButtonStatus status, int key, int x, int y);

    virtual void Keyboard(bool down, int scan);
    virtual void SystemEvent(ESysEvent se);

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
};
