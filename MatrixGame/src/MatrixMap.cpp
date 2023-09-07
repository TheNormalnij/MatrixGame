// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include <new>
#include <algorithm>

#include <windows.h>
#include "MatrixMap.hpp"
#include "MatrixMapStatic.hpp"
#include "MatrixObject.hpp"
#include "MatrixObjectBuilding.hpp"
#include "MatrixObjectCannon.hpp"
#include "MatrixRobot.hpp"
#include "MatrixRenderPipeline.hpp"
#include "MatrixFlyer.hpp"
#include "MatrixProgressBar.hpp"
#include "MatrixLoadProgress.hpp"
#include "MatrixTerSurface.hpp"
#include "MatrixSkinManager.hpp"
#include "MatrixSoundManager.hpp"
#include "Interface/MatrixHint.hpp"
#include "MatrixInstantDraw.hpp"

#include "Interface/CInterface.h"
#include "MatrixGameDll.hpp"
#include "MatrixSampleStateManager.hpp"
#include "MatrixMultiSelection.hpp"

#include "Effects/MatrixEffectLandscapeSpot.hpp"

#include "CFile.hpp"

#define RENDER_PROJ_SHADOWS_IN_STENCIL_PASS

// Globals
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

CMatrixMap::CMatrixMap()
  : CMain(), m_Console(), m_Camera(), m_IntersectFlagTracer(0), m_IntersectFlagFindObjects(0),
    m_RN(g_MatrixHeap), m_EffectsFirst(NULL), m_EffectsLast(NULL),
    m_EffectsNextTakt(NULL), m_Flags(0), m_WaterName{}, m_SkyAngle(0), m_SkyDeltaAngle(0),
    m_PrevTimeCheckStatus(-1500), m_Time(0), m_BeforeWinCount(0), m_PauseHint(NULL),
    m_DialogModeName(NULL), m_BeforeWinLooseDialogCount(0) {
    DTRACE();

    m_Reflection = NULL;

    m_Size.x = 0;
    m_Size.y = 0;
    m_SizeMove.x = 0;
    m_SizeMove.y = 0;
    m_Unit = NULL;
    m_Point = NULL;
    m_Move = NULL;

    m_VisibleCalculator = new CMatrixVisiCalc(&m_Camera);

    m_GroundZ = WATER_LEVEL;
    m_GroundZBase = WATER_LEVEL - 64;

    m_TraceStopObj = NULL;

    m_LightMain = D3DXVECTOR3(250.0f, -50.0f, -250.0f);
    D3DXVec3Normalize(&m_LightMain, &m_LightMain);

    m_Water = NULL;

    m_IdsCnt = 0;
    m_Ids = NULL;

    m_Side = NULL;
    m_SideCnt = 0;

    m_EffectsCnt = 0;

    // zakker
    D3DXMatrixIdentity(&m_Identity);

    m_EffectSpawners = NULL;
    m_EffectSpawnersCnt = 0;

    m_ShadeTime = 0;
    m_ShadeOn = false;

    m_NeutralSideColorTexture = NULL;

    // m_Reflection->Tex()->SetPriority(0xFFFFFFFF);

    CSound::Init();

    m_NextLogicObject = NULL;

    m_CameraAngle = 0;
    m_WaterNormalLen = 1;

    m_TargetMusicVolume = 1.0f;
    m_StoreCurrentMusicVolume = 1.0f;
    m_DeviceState = NULL;

    InitDificulty();

}

CMatrixMap::~CMatrixMap() {
    DTRACE();

    delete m_VisibleCalculator;

    // Clear(); do not call Clear method from this destructor!!!!!! it will be called from CMatrixMapLogic Clear
}

void CMatrixMap::InitDificulty() {
    m_Difficulty.k_damage_enemy_to_player = 1.0f;
    m_Difficulty.k_time_before_maintenance = 1.0f;
    m_Difficulty.k_friendly_fire = 1.0f;

    CBlockPar *repl = g_MatrixData->BlockGetNE(PAR_REPLACE);
    if (repl) {
        auto t1(repl->ParGetNE(PAR_REPLACE_DIFFICULTY));

        if (!t1.empty()) {
            CBlockPar *dif = g_MatrixData->BlockGetNE(PAR_SOURCE_DIFFICULTY);
            if (dif) {
                auto t2 = dif->ParGetNE(t1);
                if (!t2.empty()) {
                    m_Difficulty.k_damage_enemy_to_player = 1.0f + float(t2.GetStrPar(0, L",").GetDouble() / 100.0);
                    m_Difficulty.k_time_before_maintenance = 1.0f + float(t2.GetStrPar(1, L",").GetDouble() / 100.0);
                    m_Difficulty.k_friendly_fire = 1.0f + float(t2.GetStrPar(2, L",").GetDouble() / 100.0);
                    if (m_Difficulty.k_damage_enemy_to_player < 0.01f)
                        m_Difficulty.k_damage_enemy_to_player = 0.01f;
                    if (m_Difficulty.k_time_before_maintenance < 0.01f)
                        m_Difficulty.k_time_before_maintenance = 0.01f;
                    if (m_Difficulty.k_friendly_fire < 0.01f)
                        m_Difficulty.k_friendly_fire = 0.01f;
                }
            }
        }
    }
}

void CMatrixMap::Clear(void) {
    DTRACE();

    m_RN.Clear();

    m_Minimap.Clear();
    StaticClear();

    // clear effects

    while (m_EffectsFirst) {
#ifdef _DEBUG
        SubEffect(DEBUG_CALL_INFO, m_EffectsFirst);
#else
        SubEffect(m_EffectsFirst);
#endif
    }

    if (m_EffectSpawners) {
        for (int i = 0; i < m_EffectSpawnersCnt; ++i) {
            m_EffectSpawners[i].~CEffectSpawner();
        }
        HFree(m_EffectSpawners, g_MatrixHeap);
    }
    m_EffectSpawnersCnt = 0;
    m_EffectSpawners = NULL;
    CMatrixEffect::ClearEffects();

    GetMacroTexture().Clear();
    WaterClear();

    GetVisibleCalculator()->GroupClear();

    UnitClear();
    IdsClear();
    ClearSide();

    CTerSurface::ClearSurfaces();
    CBottomTextureUnion::Clear();
    CSkinManager::Clear();
    CSound::Clear();

    GetVisual()->ClearResources();

    m_Cursor.Clear();
    m_DI.Clear();

    if (m_DeviceState != NULL) {
        HDelete(CDeviceState, m_DeviceState, g_MatrixHeap);
        m_DeviceState = NULL;
    }
}

void CMatrixMap::IdsClear() {
    DTRACE();

    if (m_Ids) {
        for (int i = 0; i < m_IdsCnt; i++) {
            m_Ids[i].std::wstring::~wstring();
        }
        HFree(m_Ids, g_MatrixHeap);
        m_Ids = NULL;
    }
    m_IdsCnt = 0;
}

void CMatrixMap::RobotPreload(void) {
    g_LoadProgress->SetCurLP(LP_PRELOADROBOTS);
    g_LoadProgress->InitCurLP(ROBOT_HEAD_CNT + ROBOT_CHASSIS_CNT + ROBOT_WEAPON_CNT + ROBOT_ARMOR_CNT);
    int curlp = 0;

    ZeroMemory(m_RobotWeaponMatrix, sizeof(m_RobotWeaponMatrix));

    std::wstring tstr, tstr2;

    for (int i = 1; i <= ROBOT_HEAD_CNT; i++) {
        CVectorObject *vo =
                (CVectorObject *)g_Cache->Get(cc_VO, utils::format(L"%ls%d.vo", OBJECT_PATH_ROBOT_HEAD, i).c_str());

        vo->PrepareSpecial(OLF_MULTIMATERIAL_ONLY, CSkinManager::GetSkin, GSP_SIDE);

        g_LoadProgress->SetCurLPPos(curlp++);
    }
    for (int i = 1; i <= ROBOT_CHASSIS_CNT; i++) {
        CVectorObject *vo =
                (CVectorObject *)g_Cache->Get(cc_VO, utils::format(L"%ls%d.vo", OBJECT_PATH_ROBOT_CHASSIS, i).c_str());
        vo->PrepareSpecial(OLF_MULTIMATERIAL_ONLY, CSkinManager::GetSkin, GSP_SIDE);

        if (i == RUK_CHASSIS_PNEUMATIC) {
            CMatrixRobot::BuildPneumaticData(vo);
        }

        g_LoadProgress->SetCurLPPos(curlp++);
    }
    for (int i = 1; i <= ROBOT_WEAPON_CNT; i++) {
        CVectorObject *vo =
                (CVectorObject *)g_Cache->Get(cc_VO, utils::format(L"%ls%d.vo", OBJECT_PATH_ROBOT_WEAPON, i).c_str());
        vo->PrepareSpecial(OLF_MULTIMATERIAL_ONLY, CSkinManager::GetSkin, GSP_SIDE);

        g_LoadProgress->SetCurLPPos(curlp++);
    }

    for (int i = 0; i < ROBOT_ARMOR_CNT; i++) {
        CVectorObject *vo =
                (CVectorObject *)g_Cache->Get(cc_VO, utils::format(L"%ls%d.vo", OBJECT_PATH_ROBOT_ARMOR, i + 1).c_str());
        vo->PrepareSpecial(OLF_MULTIMATERIAL_ONLY, CSkinManager::GetSkin, GSP_SIDE);
        g_LoadProgress->SetCurLPPos(curlp++);

        int n = vo->GetMatrixCount();

        for (int u = 0; u < n; u++) {
            if (vo->GetMatrixId(u) < 20)
                continue;
            tstr = vo->GetMatrixName(u);

            int cntp = ParamParser{tstr}.GetCountPar(L",");
            for (int p = 0; p < cntp; p++) {
                ParamParser tstr2 = utils::trim(ParamParser{tstr}.GetStrPar(p, L","));
                if (p == 0 && tstr2 != L"W")
                    break;
                else if (p == 0) {
                    if (m_RobotWeaponMatrix[i].cnt >= ROBOT_WEAPONS_PER_ROBOT_CNT)
                        break;
                    m_RobotWeaponMatrix[i].list[m_RobotWeaponMatrix[i].cnt].id = vo->GetMatrixId(u);
                    ++m_RobotWeaponMatrix[i].cnt;
                    continue;
                }

                if (tstr2.IsOnlyInt()) {
                    if ((m_RobotWeaponMatrix[i].list[m_RobotWeaponMatrix[i].cnt - 1].access_invert & (~SETBIT(31))) ==
                        0) {
                        if (tstr2.GetInt() == RUK_WEAPON_BOMB || tstr2.GetInt() == RUK_WEAPON_MORTAR)
                            m_RobotWeaponMatrix[i].extra++;
                        else
                            m_RobotWeaponMatrix[i].common++;
                    }
                    m_RobotWeaponMatrix[i].list[m_RobotWeaponMatrix[i].cnt - 1].access_invert |=
                            (DWORD(1) << (tstr2.GetInt() - 1));
                }
                else if (tstr2 == L"I") {
                    m_RobotWeaponMatrix[i].list[m_RobotWeaponMatrix[i].cnt - 1].access_invert |= SETBIT(31);
                }
            }
        }

        // sort by id
        for (int u = 0; u < m_RobotWeaponMatrix[i].cnt - 1; u++) {
            for (int t = u + 1; t < m_RobotWeaponMatrix[i].cnt; t++) {
                if (m_RobotWeaponMatrix[i].list[u].id > m_RobotWeaponMatrix[i].list[t].id) {
                    SRobotWeaponMatrix::SW temp = m_RobotWeaponMatrix[i].list[u];
                    m_RobotWeaponMatrix[i].list[u] = m_RobotWeaponMatrix[i].list[t];
                    m_RobotWeaponMatrix[i].list[t] = temp;
                }
            }
        }
    }
}

void CMatrixMap::UnitClear(void) {
    DTRACE();

    if (m_Unit != NULL) {
        HFree(m_Unit, g_MatrixHeap);
        m_Unit = NULL;
    }
    if (m_Point != NULL) {
        HFree(m_Point, g_MatrixHeap);
        m_Point = NULL;
    }
    if (m_Move != NULL) {
        HFree(m_Move, g_MatrixHeap);
        m_Move = NULL;
    }
}

void CMatrixMap::UnitInit(int sx, int sy) {
    DTRACE();

    UnitClear();

    m_Size.x = sx;
    m_Size.y = sy;
    m_SizeMove.x = sx * MOVE_CNT;
    m_SizeMove.y = sy * MOVE_CNT;
    int cnt = sx * sy;

    m_Unit = (SMatrixMapUnit *)HAllocClear(sizeof(SMatrixMapUnit) * cnt, g_MatrixHeap);

    m_Point = (SMatrixMapPoint *)HAllocClear(sizeof(SMatrixMapPoint) * (sx + 1) * (sy + 1), g_MatrixHeap);

    m_Move = (SMatrixMapMove *)HAllocClear(sizeof(SMatrixMapMove) * m_SizeMove.x * m_SizeMove.y, g_MatrixHeap);

    /*
    SMatrixMapUnit * un=m_Unit;
    while(cnt>0) {
        un->m_TextureTop=-1;
        un++;
        cnt--;
    }
    */
}

DWORD CMatrixMap::GetColor(float wx, float wy) {
    DTRACE();

    // static int ccc = 0;
    // CDText::T("COLOR", ccc++);

    float scaledx = wx * INVERT(GLOBAL_SCALE);
    float scaledy = wy * INVERT(GLOBAL_SCALE);

    int x = TruncFloat(scaledx);
    int y = TruncFloat(scaledy);

    SMatrixMapUnit *un = UnitGetTest(x, y);
    if ((un == NULL) || (un->IsWater()))
        return GetVisual()->GetAmbientColorObj();

    float kx = scaledx - float(x);
    float ky = scaledy - float(y);

    SMatrixMapPoint *mp = PointGet(x, y);
    int temp0r, temp0g, temp0b;
    int temp1r, temp1g, temp1b;
    int temp2r, temp2g, temp2b;
    int temp3r, temp3g, temp3b;

    // color 0;
    temp0r = ((mp->color >> 16) & 255) + mp->lum_r;
    temp0g = ((mp->color >> 8) & 255) + mp->lum_g;
    temp0b = ((mp->color >> 0) & 255) + mp->lum_b;

    // color 1;
    SMatrixMapPoint *mp0 = mp + 1;
    temp1r = ((mp0->color >> 16) & 255) + mp0->lum_r;
    temp1g = ((mp0->color >> 8) & 255) + mp0->lum_g;
    temp1b = ((mp0->color >> 0) & 255) + mp0->lum_b;

    // color 2;
    mp0 = (mp + m_Size.x + 1);
    temp2r = ((mp0->color >> 16) & 255) + mp0->lum_r;
    temp2g = ((mp0->color >> 8) & 255) + mp0->lum_g;
    temp2b = ((mp0->color >> 0) & 255) + mp0->lum_b;

    // color 2;
    mp0 = (mp + m_Size.x + 2);
    temp3r = ((mp0->color >> 16) & 255) + mp0->lum_r;
    temp3g = ((mp0->color >> 8) & 255) + mp0->lum_g;
    temp3b = ((mp0->color >> 0) & 255) + mp0->lum_b;

    int r = Float2Int(LERPFLOAT(ky, LERPFLOAT(kx, temp0r, temp1r), LERPFLOAT(kx, temp2r, temp3r)));
    int g = Float2Int(LERPFLOAT(ky, LERPFLOAT(kx, temp0g, temp1g), LERPFLOAT(kx, temp2g, temp3g)));
    int b = Float2Int(LERPFLOAT(ky, LERPFLOAT(kx, temp0b, temp1b), LERPFLOAT(kx, temp2b, temp3b)));
    if (r > 255)
        r = 255;
    if (g > 255)
        g = 255;
    if (b > 255)
        b = 255;

    return (r << 16) | (g << 8) | (b);
}
float CMatrixMap::GetZInterpolatedLand(float wx, float wy) {
    DTRACE();

    int x = Float2Int((float)floor(wx * INVERT(GLOBAL_SCALE * MAP_GROUP_SIZE)));
    int y = Float2Int((float)floor(wy * INVERT(GLOBAL_SCALE * MAP_GROUP_SIZE)));

    D3DXVECTOR3 xx[4], yy[4], ox, oy;
    float fx(x * (GLOBAL_SCALE * MAP_GROUP_SIZE)), fy(y * (GLOBAL_SCALE * MAP_GROUP_SIZE)), z;

    float tx = (wx - fx) * INVERT(GLOBAL_SCALE * MAP_GROUP_SIZE);
    float ty = (wy - fy) * INVERT(GLOBAL_SCALE * MAP_GROUP_SIZE);
    if (tx < 0)
        tx = 1.0f + tx;
    if (ty < 0)
        ty = 1.0f + ty;

    for (int j = 0; j <= 3; ++j) {
        int cy = y + j - 1;
        float prex_z = GetVisibleCalculator()->GetGroupMaxZLand(x - 2, cy);
        if (prex_z < m_GroundZBaseMiddle)
            prex_z = m_GroundZBaseMiddle;
        else if (prex_z > m_GroundZBaseMax)
            prex_z = m_GroundZBaseMax;
        for (int i = 0; i <= 3; ++i) {
            int cx = x + i - 1;

            // along x
            xx[i].x = cx * (GLOBAL_SCALE * MAP_GROUP_SIZE);
            xx[i].y = cy * (GLOBAL_SCALE * MAP_GROUP_SIZE);
            z = GetVisibleCalculator()->GetGroupMaxZLand(cx, cy);
            if (z < m_GroundZBaseMiddle)
                z = m_GroundZBaseMiddle;
            else if (z > m_GroundZBaseMax)
                z = m_GroundZBaseMax;
            xx[i].z = std::max(prex_z, z);  //(prex_z + z) * 0.5f;
            prex_z = z;
        }
        SBSplineKoefs kx;
        CalcBSplineKoefs2(kx, xx[0], xx[1], xx[2], xx[3]);
        CalcBSplinePoint(kx, yy[j], tx);
    }

    SBSplineKoefs ky;
    CalcBSplineKoefs2(ky, yy[0], yy[1], yy[2], yy[3]);
    CalcBSplinePoint(ky, oy, ty);
    return oy.z;
}

float CMatrixMap::GetZInterpolatedObj(float wx, float wy) {
    DTRACE();

    int x = Float2Int((float)floor(wx * INVERT(GLOBAL_SCALE * MAP_GROUP_SIZE)));
    int y = Float2Int((float)floor(wy * INVERT(GLOBAL_SCALE * MAP_GROUP_SIZE)));

    D3DXVECTOR3 xx[4], yy[4], ox, oy;
    float fx(x * (GLOBAL_SCALE * MAP_GROUP_SIZE)), fy(y * (GLOBAL_SCALE * MAP_GROUP_SIZE)), z;

    float tx = (wx - fx) * INVERT(GLOBAL_SCALE * MAP_GROUP_SIZE);
    float ty = (wy - fy) * INVERT(GLOBAL_SCALE * MAP_GROUP_SIZE);
    if (tx < 0)
        tx = 1.0f + tx;
    if (ty < 0)
        ty = 1.0f + ty;

    for (int j = 0; j <= 3; ++j) {
        int cy = y + j - 1;
        float prex_z = GetVisibleCalculator()->GetGroupMaxZObj(x - 2, cy);
        for (int i = 0; i <= 3; ++i) {
            int cx = x + i - 1;

            // along x
            xx[i].x = cx * (GLOBAL_SCALE * MAP_GROUP_SIZE);
            xx[i].y = cy * (GLOBAL_SCALE * MAP_GROUP_SIZE);
            z = GetVisibleCalculator()->GetGroupMaxZObj(cx, cy);
            xx[i].z = std::max(prex_z, z);  //(prex_z + z) * 0.5f;
            prex_z = z;
        }
        SBSplineKoefs kx;
        CalcBSplineKoefs2(kx, xx[0], xx[1], xx[2], xx[3]);
        CalcBSplinePoint(kx, yy[j], tx);
    }

    SBSplineKoefs ky;
    CalcBSplineKoefs2(ky, yy[0], yy[1], yy[2], yy[3]);
    CalcBSplinePoint(ky, oy, ty);
    return oy.z;
}

float CMatrixMap::GetZInterpolatedObjRobots(float wx, float wy) {
    DTRACE();

    int x = Float2Int((float)floor(wx * INVERT(GLOBAL_SCALE * MAP_GROUP_SIZE)));
    int y = Float2Int((float)floor(wy * INVERT(GLOBAL_SCALE * MAP_GROUP_SIZE)));

    D3DXVECTOR3 xx[4], yy[4], ox, oy;
    float fx(x * (GLOBAL_SCALE * MAP_GROUP_SIZE)), fy(y * (GLOBAL_SCALE * MAP_GROUP_SIZE)), z;

    float tx = (wx - fx) * INVERT(GLOBAL_SCALE * MAP_GROUP_SIZE);
    float ty = (wy - fy) * INVERT(GLOBAL_SCALE * MAP_GROUP_SIZE);
    if (tx < 0)
        tx = 1.0f + tx;
    if (ty < 0)
        ty = 1.0f + ty;

    for (int j = 0; j <= 3; ++j) {
        int cy = y + j - 1;
        float prex_z = GetVisibleCalculator()->GetGroupMaxZObjRobots(x - 2, cy);
        for (int i = 0; i <= 3; ++i) {
            int cx = x + i - 1;

            // along x
            xx[i].x = cx * (GLOBAL_SCALE * MAP_GROUP_SIZE);
            xx[i].y = cy * (GLOBAL_SCALE * MAP_GROUP_SIZE);
            z = GetVisibleCalculator()->GetGroupMaxZObjRobots(cx, cy);
            xx[i].z = std::max(prex_z, z);  //(prex_z + z) * 0.5f;
            prex_z = z;
        }
        SBSplineKoefs kx;
        CalcBSplineKoefs2(kx, xx[0], xx[1], xx[2], xx[3]);
        CalcBSplinePoint(kx, yy[j], tx);
    }

    SBSplineKoefs ky;
    CalcBSplineKoefs2(ky, yy[0], yy[1], yy[2], yy[3]);
    CalcBSplinePoint(ky, oy, ty);
    return oy.z;
}

float CMatrixMap::GetZLand(double wx, double wy) {
    DTRACE();

    int x = TruncDouble(wx * INVERT(GLOBAL_SCALE));
    int y = TruncDouble(wy * INVERT(GLOBAL_SCALE));

    SMatrixMapUnit *un = UnitGetTest(x, y);
    if (un == NULL || !un->IsLand())
        return 0;

    wx -= x * GLOBAL_SCALE;
    wy -= y * GLOBAL_SCALE;

    SMatrixMapPoint *mp = PointGet(x, y);

    D3DXVECTOR3 p0, p2;
    p0.x = 0;
    p0.y = 0;
    p0.z = mp->z;
    p2.x = GLOBAL_SCALE;
    p2.y = GLOBAL_SCALE;
    p2.z = (mp + m_Size.x + 2)->z;

    if (wy < wx) {
        D3DXVECTOR3 p1;
        D3DXPLANE pl;

        p1.x = GLOBAL_SCALE;
        p1.y = 0;
        p1.z = (mp + 1)->z;

        D3DXPlaneFromPoints(&pl, &p0, &p1, &p2);
        float cc = -1.0f / pl.c;
        float a1 = pl.a * cc;
        float b1 = pl.b * cc;
        float c1 = pl.d * cc;

        return float(a1 * wx + b1 * wy + c1);
    }
    else {
        D3DXVECTOR3 p3;
        D3DXPLANE pl;
        p3.x = 0;
        p3.y = GLOBAL_SCALE;
        p3.z = (mp + m_Size.x + 1)->z;

        D3DXPlaneFromPoints(&pl, &p0, &p2, &p3);
        float cc = -1.0f / pl.c;
        float a2 = pl.a * cc;
        float b2 = pl.b * cc;
        float c2 = pl.d * cc;

        return float(a2 * wx + b2 * wy + c2);
    }
}

float CMatrixMap::GetZ(float wx, float wy) {
    DTRACE();

    int x = TruncFloat(wx * INVERT(GLOBAL_SCALE));
    int y = TruncFloat(wy * INVERT(GLOBAL_SCALE));

    SMatrixMapUnit *un = UnitGetTest(x, y);
    if (un == NULL)
        return -1000.0f;
    if (!un->IsBridge()) {
        if (un->IsWater())
            return -1000.0f;
    }
    if (un->IsFlat())
        return un->a1;

    wx -= x * GLOBAL_SCALE;
    wy -= y * GLOBAL_SCALE;

    if (wy < wx) {
        return un->a1 * wx + un->b1 * wy + un->c1;
    }
    else {
        return un->a2 * wx + un->b2 * wy + un->c2;
    }
}

void CMatrixMap::GetNormal(D3DXVECTOR3 *out, float wx, float wy, bool check_water) {
    DTRACE();

    float scaledx = wx / GLOBAL_SCALE;
    float scaledy = wy / GLOBAL_SCALE;

    int x = TruncFloat(scaledx);
    int y = TruncFloat(scaledy);

    SMatrixMapUnit *un = UnitGetTest(x, y);

    if ((un == NULL) || un->IsFlat()) {
    water:
        out->x = 0;
        out->y = 0;
        out->z = 1;
        return;
    }

    if (un->IsBridge()) {
        D3DXVECTOR3 norm(GetZ(wx - (GLOBAL_SCALE * 0.5f), wy) - GetZ(wx + (GLOBAL_SCALE * 0.5f), wy),
                         GetZ(wx, wy - (GLOBAL_SCALE * 0.5f)) - GetZ(wx, wy + (GLOBAL_SCALE * 0.5f)), GLOBAL_SCALE);
        D3DXVec3Normalize(out, &norm);
        return;
    }
    else if (un->IsWater()) {
        goto water;
    }

    float kx = scaledx - float(x);
    float ky = scaledy - float(y);

    SMatrixMapPoint *mp0 = PointGet(x, y);
    SMatrixMapPoint *mp1 = mp0 + 1;
    SMatrixMapPoint *mp2 = (mp0 + m_Size.x + 1);
    SMatrixMapPoint *mp3 = (mp0 + m_Size.x + 2);

    if (check_water && ((mp0->z < 0) || (mp1->z < 0) || (mp2->z < 0) || (mp3->z < 0)))
        goto water;

    D3DXVECTOR3 v1 = LERPVECTOR(kx, mp0->n, mp1->n);
    D3DXVECTOR3 v2 = LERPVECTOR(kx, mp2->n, mp3->n);

    auto tmp = LERPVECTOR(ky, v1, v2);
    D3DXVec3Normalize(out, &tmp);
}

bool CMatrixMap::UnitPick(const D3DXVECTOR3 &orig, const D3DXVECTOR3 &dir, const CRect &ar, int *ox, int *oy,
                          float *ot) {
    DTRACE();

    int x, y;
    D3DXVECTOR3 p0, p1, p2, p3;
    SMatrixMapUnit *un;
    float t, m_Unit, v;

    float mt = 1e30f;
    bool rv = false;

    for (y = std::max(0, ar.top); y < std::min(m_Size.y, ar.bottom); y++) {
        for (x = std::max(0, ar.left); x < std::min(m_Size.x, ar.right); x++) {
            un = UnitGet(x, y);
            if (!un->IsLand())
                continue;

            p0.x = GLOBAL_SCALE * (x);
            p0.y = GLOBAL_SCALE * (y);
            p0.z = PointGet(x, y)->z;
            p1.x = GLOBAL_SCALE * (x + 1);
            p1.y = GLOBAL_SCALE * (y);
            p1.z = PointGet(x + 1, y)->z;
            p2.x = GLOBAL_SCALE * (x + 1);
            p2.y = GLOBAL_SCALE * (y + 1);
            p2.z = PointGet(x + 1, y + 1)->z;
            p3.x = GLOBAL_SCALE * (x);
            p3.y = GLOBAL_SCALE * (y + 1);
            p3.z = PointGet(x, y + 1)->z;

            if (IntersectTriangle(orig, dir, p0, p1, p2, t, m_Unit, v) ||
                IntersectTriangle(orig, dir, p0, p2, p3, t, m_Unit, v)) {
                if (t < mt) {
                    mt = t;
                    if (ox != NULL)
                        *ox = x;
                    if (oy != NULL)
                        *oy = y;
                    if (ot != NULL)
                        *ot = mt;
                    rv = true;
                }
            }
        }
    }
    return rv;
}

bool CMatrixMap::PointPick(const D3DXVECTOR3 &orig, const D3DXVECTOR3 &dir, const CRect &ar, int *ox, int *oy) {
    DTRACE();

    int x, y;
    if (!UnitPick(orig, dir, ar, &x, &y))
        return false;

    D3DXVECTOR3 p;

    p.x = GLOBAL_SCALE * (x);
    p.y = GLOBAL_SCALE * (y);
    p.z = PointGet(x, y)->z;
    float d0 = DistLinePoint(orig, orig + dir, p);

    p.x = GLOBAL_SCALE * (x + 1);
    p.y = GLOBAL_SCALE * (y);
    p.z = PointGet(x + 1, y)->z;
    float d1 = DistLinePoint(orig, orig + dir, p);

    p.x = GLOBAL_SCALE * (x + 1);
    p.y = GLOBAL_SCALE * (y + 1);
    p.z = PointGet(x + 1, y + 1)->z;
    float d2 = DistLinePoint(orig, orig + dir, p);

    p.x = GLOBAL_SCALE * (x);
    p.y = GLOBAL_SCALE * (y + 1);
    p.z = PointGet(x, y + 1)->z;
    float d3 = DistLinePoint(orig, orig + dir, p);

    if (d1 <= d0 && d1 <= d2 && d1 <= d3)
        x++;
    else if (d2 <= d0 && d2 <= d1 && d2 <= d3) {
        x++;
        y++;
    }
    else if (d3 <= d0 && d3 <= d1 && d3 <= d2)
        y++;

    if (ox != NULL)
        *ox = x;
    if (oy != NULL)
        *oy = y;

    return true;
}

bool CMatrixMap::UnitPickGrid(const D3DXVECTOR3 &orig, const D3DXVECTOR3 &dir, int *ox, int *oy) {
    DTRACE();

    D3DXPLANE pl;
    auto tmp1 = D3DXVECTOR3(0, 0, 0);
    auto tmp2 = D3DXVECTOR3(1.0f, 0, 0);
    auto tmp3 = D3DXVECTOR3(0, 1.0f, 0);
    D3DXPlaneFromPoints(&pl, &tmp1, &tmp2, &tmp3);
    //	D3DXPlaneNormalize(&pl,&pl);

    D3DXVECTOR3 v;
    auto tmp4 = orig + dir * 1000000.0f;
    if (D3DXPlaneIntersectLine(&v, &pl, &orig, &tmp4) == NULL)
        return false;

    if (ox != NULL)
        *ox = TruncFloat(v.x / GLOBAL_SCALE);
    if (oy != NULL)
        *oy = TruncFloat(v.y / GLOBAL_SCALE);

    return true;
}

bool CMatrixMap::UnitPickWorld(const D3DXVECTOR3 &orig, const D3DXVECTOR3 &dir, float *ox, float *oy) {
    D3DXPLANE pl;
    auto tmp1 = D3DXVECTOR3(0, 0, 0);
    auto tmp2 = D3DXVECTOR3(1.0f, 0, 0);
    auto tmp3 = D3DXVECTOR3(0, 1.0f, 0);
    D3DXPlaneFromPoints(&pl, &tmp1, &tmp2, &tmp3);
    //	D3DXPlaneNormalize(&pl,&pl);

    D3DXVECTOR3 v;
    auto tmp4 = orig + dir * 1000000.0f;
    if (D3DXPlaneIntersectLine(&v, &pl, &orig, &tmp4) == NULL)
        return false;

    if (ox != NULL)
        *ox = v.x;
    if (oy != NULL)
        *oy = v.y;

    return true;
}

void CMatrixMap::StaticClear(void) {
    DTRACE();

    while (!m_AllObjects.empty())
    {
        StaticDelete(m_AllObjects.front());
    }

    CMatrixMapObject::ClearTextures();
}

#ifdef _DEBUG
#include "stdio.h"
#endif

void CMatrixMap::StaticDelete(CMatrixMapStatic *ms) {
    DTRACE();

    if (ms->IsDIP())
        return;  // already deleted
    ms->SetDIP();

    if (m_NextLogicObject == ms) {
        m_NextLogicObject = ms->GetNextLogic();
    }

    CMatrixMapStatic::RemoveFromSorted(ms);

    auto sb = m_AllObjects.begin();
    auto se = m_AllObjects.end();
    auto ses = se - 1;

    if (ms->InLT()) {
        ms->DelLT();
        // remove this object from end of array
        while (sb < se) {
            --se;
            if (*se == ms && ses != se) {
                *se = *ses;
            }
        }
    }
    else {
        // remove this object from begin of array
        while (sb < se) {
            if (*sb == ms && ses != sb) {
                *sb = *ses;
            }
            ++sb;
        }
    }
    m_AllObjects.erase(m_AllObjects.end() - 1);

    //#ifdef _DEBUG
    //    std::wstring c(L"Del obj ");
    //    if (ms->GetObjectType()==OBJECT_TYPE_MAPOBJECT) c+=L" mapobj: ";
    //	else if(ms->GetObjectType()==OBJECT_TYPE_ROBOTAI) c+=L" robot: ";
    //	else if(ms->GetObjectType()==OBJECT_TYPE_BUILDING) c+=L" build: ";
    //    else if(ms->GetObjectType()==OBJECT_TYPE_CANNON) c+=L" cannon: ";
    //    else if(ms->GetObjectType()==OBJECT_TYPE_FLYER) c+=L" flyer: ";
    //    c.AddHex(ms);
    //    c += L"\n";
    //    SLOG("objlog.txt", c.Get());
    //#endif

    if (ms->GetObjectType() == OBJECT_TYPE_MAPOBJECT) {
        HDelete(CMatrixMapObject, (CMatrixMapObject *)ms, g_MatrixHeap);
    }
    else if (ms->IsRobot()) {
        HDelete(CMatrixRobotAI, (CMatrixRobotAI *)ms, g_MatrixHeap);
    }
    else if (ms->IsCannon()) {
        HDelete(CMatrixCannon, (CMatrixCannon *)ms, g_MatrixHeap);
    }
    else if (ms->IsFlyer()) {
        HDelete(CMatrixFlyer, (CMatrixFlyer *)ms, g_MatrixHeap);
    }
    else if (ms->IsBuilding()) {
        HDelete(CMatrixBuilding, (CMatrixBuilding *)ms, g_MatrixHeap);
    }
    else
        ERROR_E;
}

// CMatrixMapStatic * CMatrixMap::StaticAdd(EObjectType type, bool add_to_logic)
//{
//    DTRACE();
//
//	CMatrixMapStatic * ms;
//	if (type==OBJECT_TYPE_MAPOBJECT) ms=HNew(g_MatrixHeap) CMatrixMapObject();
//	else if(type==OBJECT_TYPE_ROBOTAI) ms=HNew(g_MatrixHeap) CMatrixRobotAI();
//	else if(type==OBJECT_TYPE_BUILDING) ms=HNew(g_MatrixHeap) CMatrixBuilding();
//    else if(type==OBJECT_TYPE_CANNON) ms=HNew(g_MatrixHeap) CMatrixCannon();
//    else if(type==OBJECT_TYPE_FLYER) ms=HNew(g_MatrixHeap) CMatrixFlyer();
//	else ERROR_E;
//
////#ifdef _DEBUG
////    std::wstring c(L"New obj ");
////    if (type==OBJECT_TYPE_MAPOBJECT) c+=L" mapobj: ";
////	else if(type==OBJECT_TYPE_ROBOTAI) c+=L" robot: ";
////	else if(type==OBJECT_TYPE_BUILDING) c+=L" build: ";
////    else if(type==OBJECT_TYPE_CANNON) c+=L" cannon: ";
////    else if(type==OBJECT_TYPE_FLYER) c+=L" flyer: ";
////    c.AddHex(ms);
////    c += L"\n";
////    SLOG("objlog.txt", c.Get());
////#endif
//
//    m_AllObjects.push_back(ms);
//
//
//	if(add_to_logic && type!=OBJECT_TYPE_MAPOBJECT)
//    {
//		ms->AddLT();
//	}
//
//	return ms;
//}

void CMatrixMap::ClearSide() {
    DTRACE();

    if (m_Side) {
        for (int i = 0; i < m_SideCnt; i++) {
            m_Side[i].CMatrixSideUnit::~CMatrixSideUnit();
        }
        HFree(m_Side, g_MatrixHeap);
        m_Side = NULL;
    }
    m_PlayerSide = NULL;
    if (m_NeutralSideColorTexture) {
        CCache::Destroy(m_NeutralSideColorTexture);
        m_NeutralSideColorTexture = NULL;
    }
    m_SideCnt = 0;
}

// void CMatrixMap::LoadTactics(CBlockPar & bp)
//{
//    DTRACE();
//    //bp.SaveInTextFile(L"#tactics.txt");
//    for(int sides = 1; sides <= 3; sides++){
//        CMatrixSideUnit* side = GetSideById(sides);
//        side->m_TacticsPar = bp.BlockPathGet(L"Tactics");
//    }
//}

void CMatrixMap::LoadSide(CBlockPar &bp) {
    ClearSide();

    int cnt = bp.ParCount();
    ASSERT(cnt == 5);  // there are 4 sides + one neutral. please updated data.txt

    m_SideCnt = cnt - 1;
    m_Side = (CMatrixSideUnit *)HAllocClear(m_SideCnt * sizeof(CMatrixSideUnit), g_MatrixHeap);
    for (int i = 0; i < m_SideCnt; i++)
        new(&m_Side[i]) CMatrixSideUnit();

    int idx = 0;
    for (int i = 0; i < cnt; i++) {
        int id = bp.ParGetName(i).GetInt();
        const auto name = bp.ParGet(i);
        DWORD color = (DWORD(name.GetStrPar(1, L",").GetInt() & 255) << 16) | (DWORD(name.GetStrPar(2, L",").GetInt() & 255) << 8) |
                      DWORD(name.GetStrPar(3, L",").GetInt() & 255);
        DWORD colorMM = (DWORD(name.GetStrPar(5, L",").GetInt() & 255) << 16) | (DWORD(name.GetStrPar(6, L",").GetInt() & 255) << 8) |
                        DWORD(name.GetStrPar(7, L",").GetInt() & 255);

        if (id == 0) {
            m_NeutralSideColor = color;
            m_NeutralSideColorMM = colorMM;
            continue;
        }

        m_Side[idx].m_Id = id;

        m_Side[idx].m_Constructor->SetSide(id);
        m_Side[idx].m_Color = color;
        m_Side[idx].m_ColorMM = colorMM;
        m_Side[idx].m_ColorTexture = nullptr;
        m_Side[idx].m_Name = name.GetStrPar(0, L",");

        // TODO: set player side later
        if (id == DEFAULT_PLAYER_SIDE) {
            m_PlayerSide = &m_Side[idx];
            m_PlayerSide->InitPlayerSide();
        }

        ++idx;
    }
}

void CMatrixMap::SetPlayerSide(int id) {
    for (int i = 0; i < m_SideCnt; i++) {
        if (m_Side[i].m_Id == id) {
            m_PlayerSide = &m_Side[i];
            m_PlayerSide->InitPlayerSide();

            return;
        }
    }
}

void CMatrixMap::WaterClear() {
    DTRACE();

    if (m_Water) {
        HDelete(CMatrixWater, m_Water, g_MatrixHeap);
        m_Water = NULL;
    }
    GetVisibleCalculator()->GetVisWater().clear();

    SInshorewave::MarkAllBuffersNoNeed();
}

void CMatrixMap::WaterInit() {
    DTRACE();

    if (!m_Water) {
        m_Water = HNew(g_MatrixHeap) CMatrixWater;
    }
    else {
        m_Water->Clear();
    }

    GetVisibleCalculator()->GetVisWater().clear();
    m_Water->Init();
}

bool CMatrixMap::CalcVectorToLandscape(const D3DXVECTOR2 &pos, D3DXVECTOR2 &dir) {
    const float maxrad = GLOBAL_SCALE * 5;
    const float minrad = GLOBAL_SCALE * 5;
    const float radstep = GLOBAL_SCALE;

    dir.x = 0;
    dir.y = 0;

    const float up_level = 0.0f;
    const float down_level = -20.1f;

    float zz;

    for (float r = minrad; r <= maxrad; r += radstep) {
        float astep = float(INVERT(r * GLOBAL_SCALE));
        for (float a = 0; a < M_PI_MUL(2); a += astep) {
            float ca = TableCos(a);
            float sa = TableSin(a);

            float x = pos.x + r * ca;
            float y = pos.y + r * sa;

            float z = GetZ(x, y);

            if (z < down_level)
                zz = 0;
            else if (z > up_level)
                zz = 1;
            else
                zz = ((z - down_level) / (up_level - down_level));

            dir += zz * D3DXVECTOR2(ca, sa);
        }
    }

    if (dir.x == 0 && dir.y == 0) {
        return false;
    }

    D3DXVec2Normalize(&dir, &dir);
    return true;
}


void CMatrixMap::Takt(int step) {
    DTRACE();

    float fstep = float(step);

    m_SkyAngle += m_SkyDeltaAngle * step;

    int i;

    DCP();

    CMatrixMapStatic::SortEndGraphicTakt(step);

    DCP();

    for (auto group : GetVisibleCalculator()->GetVisibleGroupIterator()) {
        if (group) {
            group->GraphicTakt(step);
        }
    }

    DCP();

    CSkinManager::Takt(fstep);
    DCP();
    m_Water->Takt(step);
    DCP();
    m_Cursor.Takt(step);
    DCP();
    m_DI.Takt(step);
    DCP();

    for (i = 0; i < m_EffectSpawnersCnt; ++i) {
        m_EffectSpawners[i].Takt(fstep);
    }
    DCP();
    RemoveEffectSpawnerByTime();
    DCP();

    // SETFLAG(m_Flags,MMFLAG_EFF_TAKT);
    for (PCMatrixEffect e = m_EffectsFirst; e != NULL;) {
#ifdef DEAD_PTR_SPY_ENABLE
        CMatrixEffect *deade = (CMatrixEffect *)DeadPtr::get_dead_mem(e);
        if (deade) {
            debugbreak();
        }
#endif
#ifdef DEAD_CLASS_SPY_ENABLE
        CMatrixEffectLandscapeSpot *spot = (CMatrixEffectLandscapeSpot *)e->DCS_GetDeadBody();
        if (spot) {
            debugbreak();
        }

#endif

        m_EffectsNextTakt = e->m_Next;
        DCP();
        e->Takt(fstep);
        DCP();
        e = m_EffectsNextTakt;
    }
    DCP();
    // RESETFLAG(m_Flags,MMFLAG_EFF_TAKT);


    DCP();
    m_Minimap.Takt(fstep);
    DCP();

    CSound::Takt();

    DCP();
}

bool CMatrixMap::FindObjects(const D3DXVECTOR2 &pos, float radius, float oscale, DWORD mask, CMatrixMapStatic *skip,
                             ENUM_OBJECTS2D callback, DWORD user) {
#ifdef _DEBUG
    static int intercount = 0;

    ++intercount;
    if (intercount != 1) {
        ERROR_S(L"CMatrixMap::FindObjects cannot be called recursively");
    }
#endif

    bool hit = false;

    D3DXVECTOR2 vmin(pos.x - radius, pos.y - radius), vmax(pos.x + radius, pos.y + radius);

    int minx1, miny1, maxx1, maxy1;

    const float groupsize = GLOBAL_SCALE * MAP_GROUP_SIZE;
    const float obr_groupsize = 1.0f / groupsize;

    minx1 = TruncFloat(vmin.x * obr_groupsize);
    miny1 = TruncFloat(vmin.y * obr_groupsize);

    maxx1 = TruncFloat(vmax.x * obr_groupsize);
    maxy1 = TruncFloat(vmax.y * obr_groupsize);
    if (maxx1 < minx1) {
        maxx1 ^= minx1;
        minx1 ^= maxx1;
        maxx1 ^= minx1;
    }
    if (maxy1 < miny1) {
        maxy1 ^= miny1;
        miny1 ^= maxy1;
        maxy1 ^= miny1;
    }
    ++maxx1;
    ++maxy1;

    ++m_IntersectFlagFindObjects;
    
    const CPoint groupSizes = GetVisibleCalculator()->GetGroupSize();

    if (minx1 < 0) {
        minx1 = 0;
        if (0 > maxx1)
            goto skip;
    }

    if (maxx1 >= groupSizes.x) {
        maxx1 = groupSizes.x - 1;
        if (maxx1 < minx1)
            goto skip;
    }
    if (miny1 < 0) {
        miny1 = 0;
        if (0 > maxy1)
            goto skip;
    }
    if (maxy1 >= groupSizes.y) {
        maxy1 = groupSizes.y - 1;
        if (maxy1 < miny1)
            goto skip;
    }

    for (int x = minx1; x <= maxx1; ++x) {
        for (int y = miny1; y <= maxy1; ++y) {
            PCMatrixMapGroup g = GetVisibleCalculator()->GetGroupByIndex(x, y);
            if (g == NULL)
                continue;
            int i = 0;
            CMatrixMapStatic *ms;
            CMatrixMapStatic *ms2 = NULL;
            for (;;) {
                if (ms2 == NULL) {
                    ms = g->FindObjectAny(mask, pos, radius, oscale, i);
                }
                else {
                    ms = ms2;
                    ms2 = NULL;
                }
                if (ms == NULL)
                    break;

                if (ms->m_IntersectFlagFindObjects == m_IntersectFlagFindObjects)
                    continue;
                ms->m_IntersectFlagFindObjects = m_IntersectFlagFindObjects;

                if (ms == skip)
                    continue;
                if (ms->IsFlyer()) {
                    ms2 = ((CMatrixFlyer *)ms)->GetCarryingRobot();
                    if (ms2 != NULL) {
                        auto tmp = *(D3DXVECTOR2 *)&ms2->GetGeoCenter() - pos;
                        float dist = D3DXVec2Length(&tmp) -
                                     ms2->GetRadius() * oscale;
                        if (dist >= radius) {
                            ms2 = NULL;
                        }
                    }
                }

                hit = true;
                if (callback) {
                    if (!callback(pos, ms, user)) {
#ifdef _DEBUG
                        --intercount;
#endif

                        return hit;
                    }
                }
                else {
#ifdef _DEBUG
                    --intercount;
#endif

                    return true;
                }
            }
        }
    }

skip:;
    for (int od = 0; od < m_AlwaysDrawStorage.GetObjectCount(); ++od) {
        auto object = m_AlwaysDrawStorage.Get(od);
        if (object->m_IntersectFlagFindObjects != m_IntersectFlagFindObjects) {
            object->m_IntersectFlagFindObjects = m_IntersectFlagFindObjects;

            CMatrixMapStatic *msa[2];
            int mscnt = 1;
            msa[0] = object;

            for (;;) {
                if (mscnt == 0)
                    break;
                CMatrixMapStatic *ms = msa[0];
                if (mscnt == 2)
                    msa[0] = msa[1];
                mscnt--;

                if (ms->GetObjectType() == OBJECT_TYPE_ROBOTAI) {
                    if (((CMatrixRobot *)ms)->m_CurrState == ROBOT_DIP)
                        continue;
                    if ((mask & TRACE_ROBOT) == 0)
                        continue;
                }
                else if (ms->GetObjectType() == OBJECT_TYPE_FLYER) {
                    msa[mscnt] = ((CMatrixFlyer *)ms)->GetCarryingRobot();
                    if (msa[mscnt] != NULL) {
                        auto tmp = *(D3DXVECTOR2 *)&msa[mscnt]->GetGeoCenter() - pos;
                        float dist = D3DXVec2Length(&tmp) -
                                     msa[mscnt]->GetRadius() * oscale;
                        if (dist < radius) {
                            mscnt++;
                        }
                    }
                    if ((mask & TRACE_FLYER) == 0)
                        continue;
                    auto tmp = *(D3DXVECTOR2 *)&ms->GetGeoCenter() - pos;
                    float dist =
                            D3DXVec2Length(&tmp) - ms->GetRadius() * oscale;
                    if (dist >= radius) {
                        continue;
                    }
                }

                if (ms == skip)
                    continue;

                hit = true;
                if (callback) {
                    if (!callback(pos, ms, user)) {
#ifdef _DEBUG
                        --intercount;
#endif

                        return hit;
                    }
                }
                else {
#ifdef _DEBUG
                    --intercount;
#endif

                    return true;
                }
            }
        }
    }

#ifdef _DEBUG
    --intercount;
#endif
    return hit;
}

bool CMatrixMap::FindObjects(const D3DXVECTOR3 &pos, float radius, float oscale, DWORD mask, CMatrixMapStatic *skip,
                             ENUM_OBJECTS callback, DWORD user) {
#ifdef _DEBUG
    static int intercount = 0;

    ++intercount;
    if (intercount != 1) {
        ERROR_S(L"CMatrixMap::FindObjects cannot be called recursively");
    }
#endif

    bool hit = false;

    D3DXVECTOR2 vmin(pos.x - radius, pos.y - radius), vmax(pos.x + radius, pos.y + radius);

    int minx1, miny1, maxx1, maxy1;

    const float groupsize = GLOBAL_SCALE * MAP_GROUP_SIZE;
    const float obr_groupsize = 1.0f / groupsize;

    minx1 = TruncFloat(vmin.x * obr_groupsize);
    miny1 = TruncFloat(vmin.y * obr_groupsize);

    maxx1 = TruncFloat(vmax.x * obr_groupsize);
    maxy1 = TruncFloat(vmax.y * obr_groupsize);
    if (maxx1 < minx1) {
        maxx1 ^= minx1;
        minx1 ^= maxx1;
        maxx1 ^= minx1;
    }
    if (maxy1 < miny1) {
        maxy1 ^= miny1;
        miny1 ^= maxy1;
        maxy1 ^= miny1;
    }
    ++maxx1;
    ++maxy1;

    ++m_IntersectFlagFindObjects;

    const CPoint groupSizes = GetVisibleCalculator()->GetGroupSize();

    if (minx1 < 0) {
        minx1 = 0;
        if (0 > maxx1)
            goto skip;
    }
    if (maxx1 >= groupSizes.x) {
        maxx1 = groupSizes.x - 1;
        if (maxx1 < minx1)
            goto skip;
    }
    if (miny1 < 0) {
        miny1 = 0;
        if (0 > maxy1)
            goto skip;
    }
    if (maxy1 >= groupSizes.y) {
        maxy1 = groupSizes.y - 1;
        if (maxy1 < miny1)
            goto skip;
    }

    for (int x = minx1; x <= maxx1; ++x) {
        for (int y = miny1; y <= maxy1; ++y) {
            PCMatrixMapGroup g = GetVisibleCalculator()->GetGroupByIndex(x, y);
            if (g == NULL)
                continue;
            int i = 0;
            CMatrixMapStatic *ms;
            CMatrixMapStatic *ms2 = NULL;
            for (;;) {
                if (ms2 == NULL) {
                    ms = g->FindObjectAny(mask, pos, radius, oscale, i);
                }
                else {
                    ms = ms2;
                    ms2 = NULL;
                }
                if (ms == NULL)
                    break;

                if (ms->m_IntersectFlagFindObjects == m_IntersectFlagFindObjects)
                    continue;
                ms->m_IntersectFlagFindObjects = m_IntersectFlagFindObjects;

                if (ms == skip)
                    continue;
                if (ms->GetObjectType() == OBJECT_TYPE_FLYER) {
                    ms2 = ((CMatrixFlyer *)ms)->GetCarryingRobot();
                    if (ms2 != NULL) {
                        auto tmp = ms2->GetGeoCenter() - pos;
                        float dist = D3DXVec3Length(&tmp) - ms2->GetRadius() * oscale;
                        if (dist >= radius) {
                            ms2 = NULL;
                        }
                    }
                }

                hit = true;
                if (callback) {
                    if (!callback(pos, ms, user)) {
#ifdef _DEBUG
                        --intercount;
#endif

                        return hit;
                    }
                }
                else {
#ifdef _DEBUG
                    --intercount;
#endif

                    return true;
                }
            }
        }
    }

skip:;
    for (int od = 0; od < m_AlwaysDrawStorage.GetObjectCount(); ++od) {
        auto object = m_AlwaysDrawStorage.Get(od);
        if (object->m_IntersectFlagFindObjects != m_IntersectFlagFindObjects) {
            object->m_IntersectFlagFindObjects = m_IntersectFlagFindObjects;

            CMatrixMapStatic *msa[2];
            int mscnt = 1;
            msa[0] = object;

            for (;;) {
                if (mscnt == 0)
                    break;
                CMatrixMapStatic *ms = msa[0];
                if (mscnt == 2)
                    msa[0] = msa[1];
                mscnt--;

                if (ms->IsRobot()) {
                    if (((CMatrixRobot *)ms)->m_CurrState == ROBOT_DIP)
                        continue;
                    if ((mask & TRACE_ROBOT) == 0)
                        continue;
                }
                else if (ms->IsFlyer()) {
                    msa[mscnt] = ((CMatrixFlyer *)ms)->GetCarryingRobot();
                    if (msa[mscnt] != NULL) {
                        auto tmp = msa[mscnt]->GetGeoCenter() - pos;
                        float dist =
                                D3DXVec3Length(&tmp) - msa[mscnt]->GetRadius() * oscale;
                        if (dist < radius) {
                            mscnt++;
                        }
                    }
                    if ((mask & TRACE_FLYER) == 0)
                        continue;

                    auto tmp = ms->GetGeoCenter() - pos;
                    float dist = D3DXVec3Length(&tmp) - ms->GetRadius() * oscale;
                    if (dist >= radius) {
                        continue;
                    }
                }

                if (ms == skip)
                    continue;

                hit = true;
                if (callback) {
                    if (!callback(pos, ms, user)) {
#ifdef _DEBUG
                        --intercount;
#endif

                        return hit;
                    }
                }
                else {
#ifdef _DEBUG
                    --intercount;
#endif

                    return true;
                }
            }
        }
    }

#ifdef _DEBUG
    --intercount;
#endif
    return hit;
}

#ifdef _DEBUG
void CMatrixMap::SubEffect(const SDebugCallInfo &from, PCMatrixEffect e)
#else
void CMatrixMap::SubEffect(PCMatrixEffect e)
#endif
{
    DTRACE();
    if (e->IsDIP())
        return;  // already under destruction

#ifdef _DEBUG
    if (e->GetType() == EFFECT_LANDSCAPE_SPOT) {
        ((CMatrixEffectLandscapeSpot *)e)->BeforeRelease(from._file, from._line);
    }
#endif

    if (e->m_Next == NULL && e->m_Prev == NULL && m_EffectsFirst != e) {
        // effect not in list
        e->Release();
    }
    else {
        //        if (FLAG(m_Flags, MMFLAG_EFF_TAKT))
        //        {
        //            if (!(e->m_PrevDel==NULL && e->m_NextDel==NULL && e!=m_Effects2RemoveFirst)) return; // already
        //            dead
        //            // effect takt in progress. just put effect to dead list
        //            LIST_ADD(e, m_Effects2RemoveFirst, m_Effects2RemoveLast, m_PrevDel, m_NextDel);
        //            e->Unconnect();
        //
        ////#ifdef _DEBUG
        ////            DM("aaaaaaa","del: " + CStr(int(e->GetType())) + " from " + from._file + ":" + from._line);
        ////#endif
        //
        //        } else
        {
            if (m_EffectsNextTakt == e) {
                m_EffectsNextTakt = e->m_Next;
            }

            LIST_DEL(e, m_EffectsFirst, m_EffectsLast, m_Prev, m_Next);
            e->Release();
        }
        --m_EffectsCnt;
    }

#ifdef _DEBUG
    CDText::T("E", m_EffectsCnt);
#endif
}

// bool CMatrixMap::IsEffect(PCMatrixEffect e)
//{
//    DTRACE();
//    //for (int i=0; i<m_EffectsCnt; ++i)
//    //{
//    //    if (m_Effects[i] == e)
//    //    {
//    //        return true;
//    //    }
//    //}
//    //return false;
//    return m_Effects.Get(PTR2KEY(e),NULL);
//}

void CMatrixMap::RemoveEffectSpawnerByObject(CMatrixMapStatic *ms) {
    int i = 0;
    while (i < m_EffectSpawnersCnt) {
        if (m_EffectSpawners[i].GetUnder() == ms) {
            m_EffectSpawners[i].~CEffectSpawner();
            --m_EffectSpawnersCnt;
            if (i == m_EffectSpawnersCnt) {
                return;
            }
            m_EffectSpawners[i] = m_EffectSpawners[m_EffectSpawnersCnt];
            continue;
        }
        ++i;
    }
}

void CMatrixMap::RemoveEffectSpawnerByTime(void) {
    int i = 0;
    while (i < m_EffectSpawnersCnt) {
        if (m_EffectSpawners[i].OutOfTime())
            break;
        ++i;
    }
    if (i >= m_EffectSpawnersCnt)
        return;

    m_EffectSpawners[i].~CEffectSpawner();
    --m_EffectSpawnersCnt;
    if (i == m_EffectSpawnersCnt) {
        return;
    }
    m_EffectSpawners[i] = m_EffectSpawners[m_EffectSpawnersCnt];
}

void CMatrixMap::AddEffectSpawner(float x, float y, float z, int ttl, const std::wstring &in_par) {
    SpawnEffectSmoke smoke;
    SpawnEffectFire fire;
    SpawnEffectSound sound;
    SpawnEffectLightening lightening;

    SpawnEffectProps *props = &smoke;

    ParamParser par{in_par};
    int parcnt = par.GetCountPar(L",");
    if (parcnt < 3)
        return;
    parcnt -= 3;

    int minper = par.GetStrPar(1, L",").GetInt();
    int maxper = par.GetStrPar(2, L",").GetInt();

    int idx = 3;

    m_EffectSpawners = (CEffectSpawner *)HAllocEx(m_EffectSpawners, sizeof(CEffectSpawner) * (m_EffectSpawnersCnt + 1),
                                                  g_MatrixHeap);

    EEffectSpawnerType type = (EEffectSpawnerType)par.GetStrPar(0, L",").GetInt();

    bool light_need_action = false;

    switch (type) {
        case EST_SMOKE: {
            smoke.m_Type = EFFECT_SMOKE;

            smoke.m_ttl = (float)par.GetStrPar(idx++, L",").GetDouble();
            smoke.m_puffttl = (float)par.GetStrPar(idx++, L",").GetDouble();
            smoke.m_spawntime = (float)par.GetStrPar(idx++, L",").GetDouble();
            smoke.m_intense = par.GetStrPar(idx++, L",").GetBool();
            smoke.m_speed = (float)par.GetStrPar(idx++, L",").GetDouble();
            smoke.m_color = par.GetStrPar(idx++, L",").GetHexUnsigned();

            break;
        }
        case EST_FIRE: {
            props = &fire;

            fire.m_Type = EFFECT_FIRE;

            fire.m_ttl = (float)par.GetStrPar(idx++, L",").GetDouble();
            fire.m_puffttl = (float)par.GetStrPar(idx++, L",").GetDouble();
            fire.m_spawntime = (float)par.GetStrPar(idx++, L",").GetDouble();
            fire.m_intense = par.GetStrPar(idx++, L",").GetBool();
            fire.m_speed = (float)par.GetStrPar(idx++, L",").GetDouble();
            fire.m_dispfactor = (float)par.GetStrPar(idx++, L",").GetDouble();

            break;
        }
        case EST_SOUND: {
            props = &sound;

            sound.m_Type = EFFECT_UNDEFINED;

            sound.m_vol0 = (float)par.GetStrPar(idx++, L",").GetDouble();
            sound.m_vol1 = (float)par.GetStrPar(idx++, L",").GetDouble();
            sound.m_pan0 = (float)par.GetStrPar(idx++, L",").GetDouble();
            sound.m_pan1 = (float)par.GetStrPar(idx++, L",").GetDouble();
            sound.m_attn = (float)par.GetStrPar(idx++, L",").GetDouble();
            // sound.m_looped = par.GetStrPar(idx++,L",").GetBool();

            std::wstring nam(par.GetStrPar(idx++, L","));
            if (nam.length() > (sizeof(sound.m_name) / sizeof(sound.m_name[0]))) {
                ERROR_S(L"Effect spawner: sound name too long!");
            }

            memcpy(sound.m_name, nam.c_str(), (nam.length() + 1) * sizeof(wchar));

            break;
        }
        case EST_LIGHTENING: {
            props = &lightening;
            lightening.m_Type = EFFECT_LIGHTENING;

            lightening.m_Tag = HNew(g_MatrixHeap) std::wstring();

            *lightening.m_Tag = par.GetStrPar(idx++, L",");

            lightening.m_ttl = (float)par.GetStrPar(idx++, L",").GetDouble();
            lightening.m_Color = par.GetStrPar(idx++, L",").GetHexUnsigned();

            // set negative width. it means that m_Tag used instead of m_Pair;
            lightening.m_Width = -(float)par.GetStrPar(idx++, L",").GetDouble();
            lightening.m_Dispers = (float)par.GetStrPar(idx++, L",").GetDouble();

            // seek tag
            int bla = m_EffectSpawnersCnt - 1;
            while (bla >= 0) {
                if (m_EffectSpawners[bla].Props()->m_Type == EFFECT_LIGHTENING) {
                    SpawnEffectLightening *l = (SpawnEffectLightening *)m_EffectSpawners[bla].Props();
                    if (l->m_Width < 0) {
                        if (*l->m_Tag == *lightening.m_Tag) {
                            NEG_FLOAT(l->m_Width);
                            NEG_FLOAT(lightening.m_Width);

                            using std::wstring;
                            HDelete(wstring, l->m_Tag, g_MatrixHeap);
                            HDelete(wstring, lightening.m_Tag, g_MatrixHeap);
                            lightening.m_Pair = l;

                            lightening.m_Dispers = -1;  // me - non host

                            light_need_action = true;
                        }
                    }
                }
                --bla;
            }

            break;
        }
        default:
            ERROR_E;
    };

    props->m_Pos.x = x;
    props->m_Pos.y = y;
    props->m_Pos.z = z;

    new(&m_EffectSpawners[m_EffectSpawnersCnt]) CEffectSpawner(minper, maxper, ttl, props);

    if (light_need_action) {
        SpawnEffectLightening *l = (SpawnEffectLightening *)m_EffectSpawners[m_EffectSpawnersCnt].Props();
        l->m_Pair->m_Pair = l;
    }

    ++m_EffectSpawnersCnt;
}

void CMatrixMap::LeaveDialogMode(void) {
    if (m_DialogModeName == NULL)
        return;

    if (0 == wcscmp(m_DialogModeName, TEMPLATE_DIALOG_MENU)) {
        m_DialogModeHints[0]->SoundOut();
    }

    RESETFLAG(m_Flags, MMFLAG_DIALOG_MODE);
    Pause(false);

    for (auto item : m_DialogModeHints)
    {
        item->Release();
    }

    m_DialogModeHints.clear();
    g_IFaceList->HideHintButtons();
    m_DialogModeName = NULL;
}

static void OkHandler(void) {
    g_MatrixMap->LeaveDialogMode();
}
static void OkJustExitHandler(void) {
    g_MatrixMap->LeaveDialogMode();
    SETFLAG(g_Flags, GFLAG_EXITLOOP);
}
static void OkExitWinHandler(void) {
    g_MatrixMap->LeaveDialogMode();
    g_ExitState = 3;

    SETFLAG(g_MatrixMap->m_Flags, MMFLAG_STAT_DIALOG);
    // SETFLAG(g_Flags, GFLAG_EXITLOOP);
}
static void OkExitLooseHandler(void) {
    g_MatrixMap->LeaveDialogMode();
    g_ExitState = 2;
    SETFLAG(g_MatrixMap->m_Flags, MMFLAG_STAT_DIALOG);
    // SETFLAG(g_Flags, GFLAG_EXITLOOP);
}
static void OkExitHandler(void) {
    g_MatrixMap->LeaveDialogMode();
    g_ExitState = 1;
    SETFLAG(g_MatrixMap->m_Flags, MMFLAG_STAT_DIALOG);
    // SETFLAG(g_Flags, GFLAG_EXITLOOP);
}
static void OkSurrenderHandler(void) {
    g_MatrixMap->LeaveDialogMode();
    g_ExitState = 4;
    SETFLAG(g_MatrixMap->m_Flags, MMFLAG_STAT_DIALOG);

    for (int i = 0; i < g_MatrixMap->m_SideCnt; ++i) {
        CMatrixSideUnit *su = g_MatrixMap->m_Side + i;
        if (su->GetStatus() == SS_ACTIVE && su != g_MatrixMap->GetPlayerSide()) {
            su->SetStatValue(STAT_TIME, -su->GetStatValue(STAT_TIME));
        }
    }
    // SETFLAG(g_Flags, GFLAG_EXITLOOP);
}
static void OkResetHandler(void) {
    g_MatrixMap->LeaveDialogMode();
    g_MatrixMap->Restart();
    // g_ExitState = 4;
    // SETFLAG(g_Flags, GFLAG_EXITLOOP);
}
void ConfirmCancelHandler(void) {
    CMatrixHint *h = (CMatrixHint *)g_MatrixMap->m_DialogModeHints[1];
    g_MatrixMap->m_DialogModeHints.erase(g_MatrixMap->m_DialogModeHints.end() - 1);
    h->Release();

    g_IFaceList->EnableMainMenuButton(HINT_CANCEL_MENU);
    g_IFaceList->EnableMainMenuButton(HINT_CONTINUE);
    g_IFaceList->EnableMainMenuButton(HINT_SURRENDER);
    g_IFaceList->EnableMainMenuButton(HINT_RESET);
    g_IFaceList->EnableMainMenuButton(HINT_EXIT);

    g_IFaceList->HideHintButton(HINT_OK);
    g_IFaceList->HideHintButton(HINT_CANCEL);
}

static void CreateConfirmation(const wchar *hint, DialogButtonHandler handler) {
    g_IFaceList->DisableMainMenuButton(HINT_CANCEL_MENU);
    g_IFaceList->DisableMainMenuButton(HINT_CONTINUE);
    g_IFaceList->DisableMainMenuButton(HINT_SURRENDER);
    g_IFaceList->DisableMainMenuButton(HINT_RESET);
    g_IFaceList->DisableMainMenuButton(HINT_EXIT);

    CMatrixHint *h = CMatrixHint::Build(std::wstring(hint), hint);
    int ww = (g_ScreenX - h->m_Width) / 2;
    int hh = (g_ScreenY - h->m_Height) / 2 - Float2Int(float(g_ScreenY) * 0.09f);
    h->Show(ww, hh);

    if (h->GetCopyPosCnt() > 0) {
        int x = h->m_PosX + h->GetCopyPos(0).x;
        int y = h->m_PosY + h->GetCopyPos(0).y;
        g_IFaceList->CreateHintButton(x, y, HINT_OK, handler);
    }
    if (h->GetCopyPosCnt() > 1) {
        int x = h->m_PosX + h->GetCopyPos(1).x;
        int y = h->m_PosY + h->GetCopyPos(1).y;
        g_IFaceList->CreateHintButton(x, y, HINT_CANCEL, ConfirmCancelHandler);
    }

    g_MatrixMap->m_DialogModeHints.push_back(h);
}

void ExitRequestHandler(void) {
    CreateConfirmation(TEMPLATE_DIALOG_EXIT, OkExitHandler);
}

void ResetRequestHandler(void) {
    CreateConfirmation(TEMPLATE_DIALOG_RESET, OkResetHandler);
}

void HelpRequestHandler(void) {
    g_MatrixMap->LeaveDialogMode();
    std::wstring fn;
    if (CFile::FileExist(fn, L"Manual.exe")) {
        PROCESS_INFORMATION pi0;
        STARTUPINFOA si0;

        ZeroMemory(&si0, sizeof(si0));
        si0.cb = sizeof(si0);
        ZeroMemory(&pi0, sizeof(pi0));

        CreateProcess("Manual.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si0, &pi0);

        ShowWindow(g_Wnd, SW_MINIMIZE);

        CloseHandle(pi0.hProcess);
        CloseHandle(pi0.hThread);
    }
}

void SurrenderRequestHandler(void) {
    CreateConfirmation(TEMPLATE_DIALOG_SURRENDER, OkSurrenderHandler);
}

void CMatrixMap::EnterDialogMode(const wchar *hint_i) {
    if (m_PauseHint) {
        m_PauseHint->Release();
        m_PauseHint = NULL;
    }
    LeaveDialogMode();
    Pause(true);
    SETFLAG(m_Flags, MMFLAG_DIALOG_MODE);
    m_DialogModeName = hint_i;

    if (0 != wcscmp(hint_i, TEMPLATE_DIALOG_BEGIN)) {
        g_MatrixMap->GetPlayerSide()->PLDropAllActions();
    }

    CBlockPar *bp = g_MatrixData->BlockGet(PAR_TEMPLATES);

    int cnt = bp->ParCount();

    int ww = 20;
    int hh = 62;

    const wchar *hint = hint_i;
    if (0 == wcscmp(hint_i, TEMPLATE_DIALOG_STATISTICS_D)) {
        hint = TEMPLATE_DIALOG_STATISTICS;
    }

    for (int i = 0; i < cnt; ++i) {
        if (bp->ParGetName(i) == hint) {
            std::wstring templ(bp->ParGet(i));
            std::wstring templ2;
            if (templ[0] == '|')
                continue;

            int ii = i + 1;
            for (; ii < cnt; ++ii) {
                if (bp->ParGetName(ii) == hint) {
                    templ2 = bp->ParGet(ii);
                    if (templ2[0] == '|')
                        templ += templ2;
                }
            }

            CMatrixHint *h = CMatrixHint::Build(templ, g_MatrixData->BlockGet(PAR_REPLACE), hint);

            if (0 == wcscmp(hint, TEMPLATE_DIALOG_MENU)) {
                h->m_PosX = (g_ScreenX - h->m_Width) / 2;
                h->m_PosY = (g_ScreenY - h->m_Height) / 2 - Float2Int(float(g_ScreenY) * 0.09f);
                h->SetVisible(false);
                h->SoundIn();
            }
            else if (0 == wcscmp(hint, TEMPLATE_DIALOG_STATISTICS)) {
                h->m_PosX = (g_ScreenX - h->m_Width) / 2;
                h->m_PosY = (g_ScreenY - h->m_Height) / 2 - Float2Int(float(g_ScreenY) * 0.09f);
                h->SetVisible(true);
            }
            else {
                h->Show(ww, hh);
            }

            if (h->GetCopyPosCnt() > 0) {
                int x = h->m_PosX + h->GetCopyPos(0).x;
                int y = h->m_PosY + h->GetCopyPos(0).y;

                if (0 == wcscmp(hint, TEMPLATE_DIALOG_MENU)) {
                    g_IFaceList->CreateHintButton(x, y, HINT_CONTINUE, OkHandler);
                }
                else if (0 == wcscmp(hint, TEMPLATE_DIALOG_WIN)) {
                    g_IFaceList->CreateHintButton(x, y, HINT_OK, OkExitWinHandler);
                }
                else if (0 == wcscmp(hint, TEMPLATE_DIALOG_LOOSE)) {
                    g_IFaceList->CreateHintButton(x, y, HINT_OK, OkExitLooseHandler);
                }
                else if (0 == wcscmp(hint, TEMPLATE_DIALOG_STATISTICS)) {
                    if (0 == wcscmp(hint_i, TEMPLATE_DIALOG_STATISTICS_D)) {
                        g_IFaceList->CreateHintButton(x, y, HINT_OK, OkHandler);
                    }
                    else {
                        g_IFaceList->CreateHintButton(x, y, HINT_OK, OkJustExitHandler);
                    }
                }
                else {
                    g_IFaceList->CreateHintButton(x, y, HINT_OK, OkHandler);
                }
            }
            if (h->GetCopyPosCnt() > 1) {
                int x = h->m_PosX + h->GetCopyPos(1).x;
                int y = h->m_PosY + h->GetCopyPos(1).y;
                g_IFaceList->CreateHintButton(x, y, HINT_RESET, ResetRequestHandler);
            }
            if (h->GetCopyPosCnt() > 2) {
                int x = h->m_PosX + h->GetCopyPos(2).x;
                int y = h->m_PosY + h->GetCopyPos(2).y;
                g_IFaceList->CreateHintButton(x, y, HINT_HELP, HelpRequestHandler);
            }
            if (h->GetCopyPosCnt() > 3) {
                int x = h->m_PosX + h->GetCopyPos(3).x;
                int y = h->m_PosY + h->GetCopyPos(3).y;
                g_IFaceList->CreateHintButton(x, y, HINT_SURRENDER, SurrenderRequestHandler);
            }
            if (h->GetCopyPosCnt() > 4) {
                int x = h->m_PosX + h->GetCopyPos(4).x;
                int y = h->m_PosY + h->GetCopyPos(4).y;
                g_IFaceList->CreateHintButton(x, y, HINT_EXIT, ExitRequestHandler);
            }
            if (h->GetCopyPosCnt() > 5) {
                int x = h->m_PosX + h->GetCopyPos(5).x;
                int y = h->m_PosY + h->GetCopyPos(5).y;
                g_IFaceList->CreateHintButton(x, y, HINT_CANCEL_MENU, OkHandler);
            }

            ww += h->m_Width + 20;
            m_DialogModeHints.push_back(h);
        }
    }
}

void CMatrixMap::RestoreMusicVolume(void) {
    if (!g_RangersInterface)
        return;
    SETFLAG(m_Flags, MMFLAG_MUSIC_VOL_CHANGING);
    m_TargetMusicVolume = m_StoreCurrentMusicVolume;
}

void CMatrixMap::SetMusicVolume(float vol) {
    if (!g_RangersInterface)
        return;

    if (FLAG(m_Flags, MMFLAG_MUSIC_VOL_CHANGING)) {
        m_StoreCurrentMusicVolume = m_TargetMusicVolume;
    }
    else {
        SETFLAG(m_Flags, MMFLAG_MUSIC_VOL_CHANGING);
        m_StoreCurrentMusicVolume = g_RangersInterface->m_MusicVolumeGet();
    }
    m_TargetMusicVolume = vol;
}

void CMatrixMap::ShowPortrets(void) {
    int n = 0;
    SETFLAG(g_MatrixMap->m_Flags, MMFLAG_SHOWPORTRETS);

    for (auto item : m_AllObjects)
    {
        if (item->GetObjectType() == OBJECT_TYPE_MAPOBJECT && ((CMatrixMapObject*)item)->m_BehFlag == BEHF_PORTRET)
        {
            ((CMatrixMapObject*)item)->m_PrevStateRobotsInRadius = ++n;
            ((CMatrixMapObject*)item)->RChange(MR_Graph);
        }
    }
}

// void    CMatrixMap::BeginDieSequence(void)
//{
//    m_ShadeOn = true;
//    m_ShadeTime = SHADER_TIME;
//    m_ShadeTimeCurrent = 0;
//
//    m_ShadePosFrom = m_Camera.GetTarget();
//
//    m_ShadePosTo  = m_ShadePosFrom ; //GetPlayerSide()->GetFlyer()->GetPos();
//    m_ShadeInterfaceColor = m_Minimap.GetColor();
//}

bool CMatrixMap::IsTraceNonPlayerObj() {
    if (IS_TRACE_STOP_OBJECT(g_MatrixMap->m_TraceStopObj) &&
        (g_MatrixMap->m_TraceStopObj->IsRobot() || g_MatrixMap->m_TraceStopObj->IsBuilding() ||
         g_MatrixMap->m_TraceStopObj->IsCannon() || g_MatrixMap->m_TraceStopObj->GetObjectType() == OBJECT_TYPE_FLYER ||
         g_MatrixMap->m_TraceStopObj->IsSpecial()) &&
        (g_MatrixMap->m_TraceStopObj->GetSide() != PLAYER_SIDE))
        return true;

    return false;
}
