// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CRandom.hpp"

namespace Base {

CRandom::CRandom() {
    m_Rnd = 0;
}

int CRandom::Rnd() {
    m_Rnd = 16807 * (m_Rnd % 127773) - 2836 * (m_Rnd / 127773);
    if (m_Rnd <= 0)
        m_Rnd = m_Rnd + 2147483647;
    return m_Rnd - 1;
}

double CRandom::RndFloat(void) {
    return float(Rnd()) / float(2147483647 - 2);
}

int CRandom::Rnd(int zmin, int zmax) {
    if (zmin <= zmax)
        return zmin + (Rnd() % (zmax - zmin + 1));
    else
        return zmax + (Rnd() % (zmin - zmax + 1));
}

double CRandom::RndFloat(double zmin, double zmax) {
    return zmin + RndFloat() * (zmax - zmin);
}

}  // namespace Base