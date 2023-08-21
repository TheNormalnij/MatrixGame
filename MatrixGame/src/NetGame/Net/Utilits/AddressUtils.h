// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <string_view>

namespace AddressUtils {
    unsigned short GetPortFromAddress(std::string_view host, unsigned short defaultPort = 0) noexcept;
}
