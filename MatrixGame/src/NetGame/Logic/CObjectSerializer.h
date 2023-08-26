// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2023, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "MatrixObjectBuilding.hpp"

// Сериализует и десериализует объекты для передачи по сети
class CObjectSerializer {
public:
    CObjectSerializer() = default;
    ~CObjectSerializer() = default;

	void SerializeBaseToPos(CMatrixBuilding *pBase, float &x, float &y) const noexcept;
    void DeserializeBaseFromPos(CMatrixBuilding **pBase, float x, float y) const noexcept;
};