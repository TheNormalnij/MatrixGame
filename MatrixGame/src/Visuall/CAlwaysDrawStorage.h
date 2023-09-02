// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "MatrixMapStatic.hpp"
#include <array>

constexpr auto MAX_ALWAYS_DRAW_OBJ = 16;

// TODO nice to have iterator here
class CAlwaysDrawStorage {
public:
    CAlwaysDrawStorage();
    ~CAlwaysDrawStorage() = default;

    bool HasFreeSpace() const noexcept { return m_AD_Obj_cnt < MAX_ALWAYS_DRAW_OBJ; };
    void Add(CMatrixMapStatic *ms);
    void Remove(CMatrixMapStatic *ms);

    int GetObjectCount() const noexcept { return m_AD_Obj_cnt; };
    CMatrixMapStatic *Get(int index) const noexcept { return m_AD_Obj[index]; };

private:
    std::array<CMatrixMapStatic *, MAX_ALWAYS_DRAW_OBJ> m_AD_Obj; // out of land object // flyer
    int m_AD_Obj_cnt;
};
