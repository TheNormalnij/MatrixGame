// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2023, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "Control/IOrderProcessor.h"
#include "CObjectSerializer.h"
#include "../Net/CServerApi.h"

// Класс для обработки приказов игрока и отправки на сервер
// для синхронизации всеми игроками

class CNetOrderProcessor : public IOrderProcessor {
public:
    CNetOrderProcessor(CMatrixSideUnit *side, CServerAPI *api) : m_pSide(side), m_pServerApi(api){};
    ~CNetOrderProcessor() = default;

    // IOrderProcessor interface

    void BuildTurret(CMatrixBuilding *pParentBase, float posX, float posY, float angle, int place,
                     int m_cannonId) override;

    
    void BuildRobot(CMatrixBuilding *pParentBase, SRobotCostructInfo &info, int count) override;

private:
    CMatrixSideUnit *m_pSide;
    CObjectSerializer m_serializer;
    CServerAPI *m_pServerApi;
};