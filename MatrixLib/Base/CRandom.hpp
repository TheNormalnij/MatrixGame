// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

namespace Base {

class CRandom {
public:
    CRandom();
    ~CRandom() = default;
    void Reset(int seed = 0) { m_Rnd = seed; };
    int Rnd();
    double RndFloat();  // 0.0 - 1.0
    int Rnd(int zmin, int zmax);
    double RndFloat(double zmin, double zmax);

private:
    int m_Rnd;
};

}  // namespace Base
