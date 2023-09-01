// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <cstdint>

#define MAX_SCANS             16
#define MOUSE_BORDER          4
#define DOUBLESCAN_TIME_DELTA 200

struct SKeyScan {
    SKeyScan() = default;

    SKeyScan(uint32_t _time, uint32_t _scan) : time{_time}, scan{_scan} {}

    uint32_t time;
    uint32_t scan;
};
