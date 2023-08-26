// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2023, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "IOrderProcessor.h"

// Класс предназначен для получения от игрока игровых приказов
// и вывода их на локальный или сетевой обработчик
class COrderController {
public:
    COrderController(IOrderProcessor *processor, CMatrixSideUnit *playerSide);
    ~COrderController() = default;

    void OnRButtonDown(const CPoint &mouse);
    void OnRButtonDouble(const CPoint &mouse);
    void OnLButtonDown(const CPoint &mouse);
    void OnLButtonDouble(const CPoint &mouse);
    void OnRButtonUp(const CPoint &mouse);
    void OnLButtonUp(const CPoint &mouse);
    void OnForward(bool down);
    void OnBackward(bool down);
    void OnLeft(bool down);
    void OnRight(bool down);
    void OnMouseMove();

private:
    bool IsArcadeMode() const noexcept { return m_pPlayerSide->IsArcadeMode(); };

private:
    IOrderProcessor *m_pOrderProcessor;
    CMatrixSideUnit *m_pPlayerSide;
};
