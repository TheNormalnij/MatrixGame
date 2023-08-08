// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "AddressUtils.h"
#include <string>

unsigned short AddressUtils::GetPortFromAddress(std::string_view address, unsigned short defaultPort) noexcept {
    const size_t portStart = address.find(":", 0) + 1;

    if (portStart == 0) {
        return defaultPort;
    }

    if ((address.length() - portStart) > 5 || (address.length() - portStart) < 0) {
        return 0;
    }

    size_t pos;

    const int port = std::stoi(&address[portStart + 1], &pos);

    return port;
}