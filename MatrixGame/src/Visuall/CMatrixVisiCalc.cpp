// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CMatrixVisiCalc.h"

#include "MatrixMap.hpp"

#include <algorithm>
#include <vector>

CMatrixVisiCalc::CMatrixVisiCalc(CMatrixCamera *pCamera) {
    m_minz = 10000.0;
    m_maxz = -10000.0;
    m_Camera = pCamera;

    m_GroupSize.x = 0;
    m_GroupSize.y = 0;
    m_Group = nullptr;
    m_GroupVis = nullptr;
    m_VisibleGroups = nullptr;

    m_VisibleGroupsCount = 0;
}

#define NPOS 4

void CMatrixVisiCalc::CheckCandidate(SCalcVisRuntime &visRuntime, CMatrixMapGroup *mapGroup) {
    int i0 = NPOS - 1;
    int i1 = 0;

    if (mapGroup->IsPointIn(visRuntime.pos[0]))
        goto visible;
    if (mapGroup->IsPointIn(visRuntime.pos[1]))
        goto visible;
    if (mapGroup->IsPointIn(visRuntime.pos[2]))
        goto visible;
#if NPOS == 4
    if (mapGroup->IsPointIn(visRuntime.pos[3]))
        goto visible;
#endif

    while (i1 < NPOS) {
        if (PointLineCatch(visRuntime.pos[i0], visRuntime.pos[i1], mapGroup->GetPos0())) {
            goto checknext;
        }
        if (PointLineCatch(visRuntime.pos[i0], visRuntime.pos[i1], mapGroup->GetPos1())) {
            goto checknext;
        }
        if (PointLineCatch(visRuntime.pos[i0], visRuntime.pos[i1],
                           D3DXVECTOR2(mapGroup->GetPos0().x, mapGroup->GetPos1().y))) {
            goto checknext;
        }
        if (PointLineCatch(visRuntime.pos[i0], visRuntime.pos[i1],
                           D3DXVECTOR2(mapGroup->GetPos1().x, mapGroup->GetPos0().y))) {
            goto checknext;
        }
        goto invisible;
    checknext:
        i0 = i1++;
    }

    // last check : frustum
    if (mapGroup->IsInFrustum()) {
    visible:
        // cmg->SetVisible(true);
        (*visRuntime.mapGroup) = mapGroup;
        ++m_VisibleGroupsCount;
        ++visRuntime.mapGroup;
    }
    else {
    invisible:
        // cmg->SetVisible(false);
        // so, map group is invisible.
        // if it is an edge of whole map, it's water can be visible
        if ((visRuntime.i == (m_GroupSize.x - 1)) || (visRuntime.j == (m_GroupSize.y - 1))) {
            D3DXVECTOR3 mins(mapGroup->GetPos0().x, mapGroup->GetPos0().y, WATER_LEVEL);
            D3DXVECTOR3 maxs(mins.x + float(MAP_GROUP_SIZE * GLOBAL_SCALE),
                             mins.y + float(MAP_GROUP_SIZE * GLOBAL_SCALE), WATER_LEVEL);
            if (m_Camera->IsInFrustum(mins, maxs)) {
                m_VisWater.push_back(mapGroup->GetPos0());
            }
        }
    }
}

// visibility
void CMatrixVisiCalc::CalcMapGroupVisibility(void) {
    DTRACE();

    SCalcVisRuntime visRuntime;

    D3DXVECTOR3 topForward(m_Camera->GetFrustumLT() + m_Camera->GetFrustumRT());
    float cameraDirection2d = D3DXVec2Length((D3DXVECTOR2 *)&topForward);
    float k_cam = (float)fabs(topForward.z) / cameraDirection2d;
    float k_etalon = m_Camera->GetFrustumCenter().z * INVERT(MAX_VIEW_DISTANCE);
    float k_etalon_fog = m_Camera->GetFrustumCenter().z * INVERT(MAX_VIEW_DISTANCE * FOG_NEAR_K);
    // float dist_naklon = (float)sqrt(dist + m_Camera->GetFrustumCenter().z * m_Camera->GetFrustumCenter().z);

    if (topForward.z > 0 || k_cam < k_etalon_fog) {
        SETFLAG(g_MatrixMap->m_Flags, MMFLAG_FOG_ON);
    }
    else {
        RESETFLAG(g_MatrixMap->m_Flags, MMFLAG_FOG_ON);
    }

    if (k_cam < k_etalon || topForward.z > 0) {
        // float d2 = dist * 2;
        float k;
        // right top <- left top
        if (m_Camera->GetFrustumLT().z >= (-0.001)) {
            k = MAX_VIEW_DISTANCE;
        }
        else {
            k = (m_minz - m_Camera->GetFrustumCenter().z) / m_Camera->GetFrustumLT().z;
            if (k > MAX_VIEW_DISTANCE)
                k = MAX_VIEW_DISTANCE;
        }
        visRuntime.pos[0].x = m_Camera->GetFrustumCenter().x + m_Camera->GetFrustumLT().x * k;
        visRuntime.pos[0].y = m_Camera->GetFrustumCenter().y + m_Camera->GetFrustumLT().y * k;

        if (m_Camera->GetFrustumRT().z >= (-0.001)) {
            k = MAX_VIEW_DISTANCE;
        }
        else {
            k = (m_minz - m_Camera->GetFrustumCenter().z) / m_Camera->GetFrustumRT().z;
            if (k > MAX_VIEW_DISTANCE)
                k = MAX_VIEW_DISTANCE;
        }
        visRuntime.pos[1].x = m_Camera->GetFrustumCenter().x + m_Camera->GetFrustumRT().x * k;
        visRuntime.pos[1].y = m_Camera->GetFrustumCenter().y + m_Camera->GetFrustumRT().y * k;

        D3DXVECTOR2 ex;
        auto tmp = visRuntime.pos[1] - visRuntime.pos[0];
        D3DXVec2Normalize(&ex, &tmp);
        ex *= GLOBAL_SCALE * MAP_GROUP_SIZE;
        visRuntime.pos[0] -= ex;
        visRuntime.pos[1] += ex;

        k = (m_minz - m_Camera->GetFrustumCenter().z) / m_Camera->GetFrustumRB().z;
        visRuntime.pos[2].x = m_Camera->GetFrustumCenter().x + m_Camera->GetFrustumRB().x * k;
        visRuntime.pos[2].y = m_Camera->GetFrustumCenter().y + m_Camera->GetFrustumRB().y * k;

        k = (m_minz - m_Camera->GetFrustumCenter().z) / m_Camera->GetFrustumLB().z;
        visRuntime.pos[3].x = m_Camera->GetFrustumCenter().x + m_Camera->GetFrustumLB().x * k;
        visRuntime.pos[3].y = m_Camera->GetFrustumCenter().y + m_Camera->GetFrustumLB().y * k;

        D3DXVECTOR2 disp(*(D3DXVECTOR2 *)&m_Camera->GetFrustumCenter() - (visRuntime.pos[2] + visRuntime.pos[3]) * 0.5f);
        visRuntime.pos[2] += disp;
        visRuntime.pos[3] += disp;

        // enable fog
        SETFLAG(g_MatrixMap->m_Flags, MMFLAG_SKY_ON);

        {
            D3DXVECTOR3 projection(m_Camera->GetDir());
            projection.z = 0;
            float len = D3DXVec3Length(&projection);
            if (len < 0.0001) {
                g_MatrixMap->m_SkyHeight = -100;
            }
            else {
                projection *= (1.0f / len);
                projection = m_Camera->GetFrustumCenter() + projection * MAX_VIEW_DISTANCE;
                projection.z -= m_Camera->GetFrustumCenter().z * 1.5f;

                D3DXVECTOR2 ptr = m_Camera->Project(projection, g_MatrixMap->GetIdentityMatrix());
                g_MatrixMap->m_SkyHeight = ptr.y;
            }
        }
    }
    else {
        RESETFLAG(g_MatrixMap->m_Flags, MMFLAG_SKY_ON);

        float k;

        // right top <- left top
        k = (m_minz - m_Camera->GetFrustumCenter().z) / m_Camera->GetFrustumLT().z;
        visRuntime.pos[0].x = m_Camera->GetFrustumCenter().x + m_Camera->GetFrustumLT().x * k;
        visRuntime.pos[0].y = m_Camera->GetFrustumCenter().y + m_Camera->GetFrustumLT().y * k;

        k = (m_minz - m_Camera->GetFrustumCenter().z) / m_Camera->GetFrustumRT().z;
        visRuntime.pos[1].x = m_Camera->GetFrustumCenter().x + m_Camera->GetFrustumRT().x * k;
        visRuntime.pos[1].y = m_Camera->GetFrustumCenter().y + m_Camera->GetFrustumRT().y * k;

        k = (m_minz - m_Camera->GetFrustumCenter().z) / m_Camera->GetFrustumRB().z;
        visRuntime.pos[2].x = m_Camera->GetFrustumCenter().x + m_Camera->GetFrustumRB().x * k;
        visRuntime.pos[2].y = m_Camera->GetFrustumCenter().y + m_Camera->GetFrustumRB().y * k;

        k = (m_minz - m_Camera->GetFrustumCenter().z) / m_Camera->GetFrustumLB().z;
        visRuntime.pos[3].x = m_Camera->GetFrustumCenter().x + m_Camera->GetFrustumLB().x * k;
        visRuntime.pos[3].y = m_Camera->GetFrustumCenter().y + m_Camera->GetFrustumLB().y * k;

        if (m_Camera->GetFrustPlaneB().norm.z > 0) {
            D3DXVECTOR2 disp(*(D3DXVECTOR2 *)&m_Camera->GetFrustumCenter() -
                             (visRuntime.pos[2] + visRuntime.pos[3]) * 0.5f);
            visRuntime.pos[2] += disp;
            visRuntime.pos[3] += disp;
        }
    }

    bool no_info_about_visibility = true;

    visRuntime.mapGroup = m_VisibleGroups;
    m_VisibleGroupsCount = 0;

    if (m_GroupVis != NULL) {
        const D3DXVECTOR3 &cameraPoint = m_Camera->GetFrustumCenter();
        int gx = TruncDouble(cameraPoint.x * (1.0 / (GLOBAL_SCALE * MAP_GROUP_SIZE)));
        int gy = TruncDouble(cameraPoint.y * (1.0 / (GLOBAL_SCALE * MAP_GROUP_SIZE)));

        if (gx >= 0 && gx < m_GroupSize.x && gy >= 0 && gy < m_GroupSize.y) {
            SGroupVisibility *visibilityGroup = m_GroupVis + gx + gy * m_GroupSize.x;

            int level = TruncFloat((cameraPoint.z - visibilityGroup->z_from) / GLOBAL_SCALE);

            if (level >= 0 && level < visibilityGroup->levels_cnt) {
                int n = visibilityGroup->levels[level];

                for (int i = 0; i < n; ++i) {
                    PCMatrixMapGroup g = visibilityGroup->vis[i];

                    visRuntime.i = Float2Int(g->GetPos0().x) / int(MAP_GROUP_SIZE * GLOBAL_SCALE);
                    visRuntime.j = Float2Int(g->GetPos0().y) / int(MAP_GROUP_SIZE * GLOBAL_SCALE);

                    CheckCandidate(visRuntime, g);
                }

                no_info_about_visibility = false;
            }
        }
    }

    float minx = visRuntime.pos[0].x;
    float maxx = visRuntime.pos[0].x;
    float miny = visRuntime.pos[0].y;
    float maxy = visRuntime.pos[0].y;
    for (int k = 1; k < NPOS; ++k) {
        if (visRuntime.pos[k].x < minx)
            minx = visRuntime.pos[k].x;
        if (visRuntime.pos[k].x > maxx)
            maxx = visRuntime.pos[k].x;
        if (visRuntime.pos[k].y < miny)
            miny = visRuntime.pos[k].y;
        if (visRuntime.pos[k].y > maxy)
            maxy = visRuntime.pos[k].y;
    }

    int iminx = (int)floor(minx * (1.0 / (MAP_GROUP_SIZE * GLOBAL_SCALE))) - 1;
    int imaxx = (int)(maxx * (1.0 / (MAP_GROUP_SIZE * GLOBAL_SCALE))) + 1;
    int iminy = (int)floor(miny * (1.0 / (MAP_GROUP_SIZE * GLOBAL_SCALE))) - 1;
    int imaxy = (int)(maxy * (1.0 / (MAP_GROUP_SIZE * GLOBAL_SCALE))) + 1;

    m_VisWater.clear();

    // DM("TIME", "TIME_STEP1 " + CStr(iminx) + " " + CStr(iminy)+ " " + CStr(imaxx)+ " " + CStr(imaxy));

    for (visRuntime.j = iminy; visRuntime.j <= imaxy /*m_GroupSize.y*/; ++visRuntime.j) {
        for (visRuntime.i = iminx; visRuntime.i <= imaxx /*m_GroupSize.x*/; ++visRuntime.i) {
            bool is_map = (visRuntime.i >= 0) && (visRuntime.i < m_GroupSize.x) && (visRuntime.j >= 0) &&
                          (visRuntime.j < m_GroupSize.y);
            if (is_map) {
                // calculate visibility of map group
                CMatrixMapGroup *cmg = m_Group[visRuntime.j * m_GroupSize.x + visRuntime.i];

                if (cmg == NULL) {
                    goto water_calc;
                }
                else {
                    if (no_info_about_visibility)
                        CheckCandidate(visRuntime, cmg);
                }
            }
            else {
                // calculate visibility of free water
            water_calc:
                D3DXVECTOR3 p0(float(visRuntime.i * (MAP_GROUP_SIZE * GLOBAL_SCALE)),
                               float(visRuntime.j * (MAP_GROUP_SIZE * GLOBAL_SCALE)), WATER_LEVEL);
                D3DXVECTOR3 p1(p0.x + float(MAP_GROUP_SIZE * GLOBAL_SCALE), p0.y + float(MAP_GROUP_SIZE * GLOBAL_SCALE),
                               WATER_LEVEL);

                int i0 = NPOS - 1;
                int i1 = 0;
                while (i1 < NPOS) {
                    if (PointLineCatch(visRuntime.pos[i0], visRuntime.pos[i1], *(D3DXVECTOR2 *)&p0)) {
                        goto checknext_w;
                    }
                    if (PointLineCatch(visRuntime.pos[i0], visRuntime.pos[i1], *(D3DXVECTOR2 *)&p1)) {
                        goto checknext_w;
                    }
                    if (PointLineCatch(visRuntime.pos[i0], visRuntime.pos[i1], D3DXVECTOR2(p0.x, p1.y))) {
                        goto checknext_w;
                    }
                    if (PointLineCatch(visRuntime.pos[i0], visRuntime.pos[i1], D3DXVECTOR2(p1.x, p0.y))) {
                        goto checknext_w;
                    }
                    goto invisible_w;
                checknext_w:
                    i0 = i1++;
                }

                if (m_Camera->IsInFrustum(p0, p1)) {
                    m_VisWater.push_back(*(D3DXVECTOR2 *)&p0);
                }
            invisible_w:;
            }
        }
    }

    // DM("TIME", "TIME_END");
}


void CMatrixVisiCalc::ClearGroupVis() {
    if (m_GroupVis != NULL) {
        int cnt = m_GroupSize.x * m_GroupSize.y;
        SGroupVisibility *md = m_GroupVis;
        while (cnt > 0) {
            md->Release();
            md++;
            cnt--;
        }
        HFree(m_GroupVis, g_MatrixHeap);
        m_GroupVis = NULL;
    }
}

void CMatrixVisiCalc::GroupClear() {
    DTRACE();

    ClearGroupVis();

    if (m_Group != NULL) {
        int cnt = m_GroupSize.x * m_GroupSize.y;
        CMatrixMapGroup **md = m_Group;
        while (cnt > 0) {
            if (*md) {
                HDelete(CMatrixMapGroup, *md, g_MatrixHeap);
            }
            md++;
            cnt--;
        }
        HFree(m_Group, g_MatrixHeap);
        HFree(m_VisibleGroups, g_MatrixHeap);

        m_Group = NULL;
    }
    m_GroupSize.x = 0;
    m_GroupSize.y = 0;
}

float CMatrixVisiCalc::GetGroupMaxZLand(int x, int y) const noexcept {
    if (x < 0 || x >= m_GroupSize.x || y < 0 || y >= m_GroupSize.y)
        return 0;
    CMatrixMapGroup *g = GetGroupByIndex(x, y);
    if (g == NULL)
        return 0;
    float z = GetGroupByIndex(x, y)->GetMaxZLand();
    if (z < 0)
        return 0;
    return z;
}

float CMatrixVisiCalc::GetGroupMaxZObj(int x, int y) const noexcept {
    if (x < 0 || x >= m_GroupSize.x || y < 0 || y >= m_GroupSize.y)
        return 0;
    CMatrixMapGroup *g = GetGroupByIndex(x, y);
    if (g == NULL)
        return 0;
    float z = GetGroupByIndex(x, y)->GetMaxZObj();
    if (z < 0)
        return 0;
    return z;
}

float CMatrixVisiCalc::GetGroupMaxZObjRobots(int x, int y) const noexcept {
    if (x < 0 || x >= m_GroupSize.x || y < 0 || y >= m_GroupSize.y)
        return 0;
    CMatrixMapGroup *g = GetGroupByIndex(x, y);
    if (g == NULL)
        return 0;
    float z = GetGroupByIndex(x, y)->GetMaxZObjRobots();
    if (z < 0)
        return 0;
    return z;
}

void CMatrixVisiCalc::Init(int x, int y) {
    GroupClear();

    m_GroupSize.x = TruncFloat((x + MAP_GROUP_SIZE - 1) * INVERT(MAP_GROUP_SIZE));
    m_GroupSize.y = TruncFloat((y + MAP_GROUP_SIZE - 1) * INVERT(MAP_GROUP_SIZE));

    int cnt = m_GroupSize.x * m_GroupSize.y;

    m_Group = (CMatrixMapGroup **)HAllocClear(cnt * sizeof(CMatrixMapGroup **), g_MatrixHeap);
    m_VisibleGroups = (CMatrixMapGroup **)HAlloc(cnt * sizeof(CMatrixMapGroup **), g_MatrixHeap);
}

void CMatrixVisiCalc::InitVisibilytyGroups() {
    m_GroupVis = (SGroupVisibility *)HAllocClear(sizeof(SGroupVisibility) * GetGroupCount(), g_MatrixHeap);
}

void CMatrixVisiCalc::Build(int x, int y, unsigned char *g) {
    int disp = (x / MAP_GROUP_SIZE) + (y / MAP_GROUP_SIZE) * m_GroupSize.x;
    m_Group[disp] = HNew(g_MatrixHeap) CMatrixMapGroup();
    m_Group[disp]->BuildBottom(x, y, g);
}
