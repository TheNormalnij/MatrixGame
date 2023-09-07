// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "MatrixMapStatic.hpp"
#include "CP2PInterator.h"
#include "MatrixCamera.hpp"

struct SCalcVisRuntime {
    D3DXVECTOR2 pos[4];
    PCMatrixMapGroup *mapGroup;
    int i, j;
};

struct SGroupVisibility {
    PCMatrixMapGroup *vis;
    int vis_cnt;
    float z_from;
    int *levels;
    int levels_cnt;

    void Release(void);
};

class CMatrixVisiCalc {
public:
    CMatrixVisiCalc(CMatrixCamera *pCamera);
    ~CMatrixVisiCalc() = default;

    void CheckCandidate(SCalcVisRuntime &visRuntime, CMatrixMapGroup *mapGroup);
    void CalcMapGroupVisibility();

    void ClearGroupVis();
    void GroupClear();

    inline CMatrixMapGroup *GetGroupByIndex(int x, int y) const noexcept { return m_Group[x + y * m_GroupSize.x]; }
    inline CMatrixMapGroup *GetGroupByIndex(int i) const noexcept { return m_Group[i]; }
    inline CMatrixMapGroup *GetGroupByIndexTest(int x, int y) const noexcept {
        return (x < 0 || y < 0 || x >= m_GroupSize.x || y >= m_GroupSize.x) ? NULL : m_Group[x + y * m_GroupSize.x];
    }

    inline SGroupVisibility *GetGroupVisByIndex(int i) const noexcept { return &m_GroupVis[i]; }
    inline SGroupVisibility *GetGroupVisByIndex(int x, int y) const noexcept {
        return m_GroupVis + x + y * m_GroupSize.x;
    };

    float GetGroupMaxZLand(int x, int y) const noexcept;
    float GetGroupMaxZObj(int x, int y) const noexcept;
    float GetGroupMaxZObjRobots(int x, int y) const noexcept;

    void ExpandZ(float z) noexcept {
        if (z < m_minz)
            m_minz = z;
        if (z > m_maxz)
            m_maxz = z;
    };

    int GetGroupCount() const noexcept { return m_GroupSize.y * m_GroupSize.x; };
    float GetSkyHeight() const noexcept { return m_SkyHeight; };

    void Init(int x, int y);
    void InitVisibilytyGroups();

    std::vector<D3DXVECTOR2> &GetVisWater() noexcept { return m_VisWater; };
    CPoint GetGroupSize() const noexcept { return m_GroupSize; };

    void Build(int x, int y, unsigned char *b);

    auto GetGroupsIterator() const noexcept { return CP2PInterator(m_Group, GetGroupCount()); };
    auto GetVisibleGroupIterator() const noexcept { return CP2PInterator(m_VisibleGroups, m_VisibleGroupsCount); };

private:
    float m_minz;
    float m_maxz;
    float m_SkyHeight;

    CMatrixCamera *m_Camera;

    CPoint m_GroupSize;
    CMatrixMapGroup **m_Group;
    SGroupVisibility *m_GroupVis;

    CMatrixMapGroup **m_VisibleGroups;
    int m_VisibleGroupsCount;
    std::vector<D3DXVECTOR2> m_VisWater;
};
