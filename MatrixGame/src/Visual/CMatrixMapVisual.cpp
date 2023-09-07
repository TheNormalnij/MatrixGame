// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CMatrixMapVisual.h"

#include "MatrixMap.hpp"
#include "MatrixSampleStateManager.hpp"
#include "Interface/CInterface.h"
#include "MatrixTerSurface.hpp"
#include "ShadowStencil.hpp"
#include "MatrixMultiSelection.hpp"
#include "MatrixRenderPipeline.hpp"
#include "Interface/MatrixHint.hpp"
#include "Effects/MatrixEffectLandscapeSpot.hpp"
#include "MatrixObjectCannon.hpp"

CMatrixMapVisual::CMatrixMapVisual(CMatrixMap *map) {
    m_Map = map;
    m_Cursor = &map->m_Cursor;
    m_Camera = &map->m_Camera;
    m_DI = &map->m_DI;
    m_AlwaysDrawStorage = &map->GetAlwaysDrawStorage();

    m_CurFrame = 0;
    m_SkyTex = {0};
    m_ShadowVB = nullptr;

    m_BiasTer = -1.0f;
    m_BiasWater = -1.0f;
    m_BiasRobots = -1.0f;

    m_AmbientColorObj = 0xFF000000;
    m_ShadowColor = 0xFF000000;
    m_SkyColor = 0xFF000000;
}

void CMatrixMapVisual::ClearResources() {
    if (IS_VB(m_ShadowVB))
        DESTROY_VB(m_ShadowVB);
}

void CMatrixMapVisual::CreatePoolDefaultResources() {
    SShadowRectVertex *v;
    if (IS_VB(m_ShadowVB))
        DESTROY_VB(m_ShadowVB);
    CREATE_VB(sizeof(SShadowRectVertex) * 4, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, m_ShadowVB);
    LOCK_VB(m_ShadowVB, &v);

    const auto shadowColor = GetShadowColor();

    v[0].p = D3DXVECTOR4(0, float(g_ScreenY), 0.0f, 1.0f);
    v[0].color = shadowColor;
    v[1].p = D3DXVECTOR4(0, 0, 0.0f, 1.0f);
    v[1].color = shadowColor;
    v[2].p = D3DXVECTOR4(float(g_ScreenX), float(g_ScreenY), 0.0f, 1.0f);
    v[2].color = shadowColor;
    v[3].p = D3DXVECTOR4(float(g_ScreenX), 0, 0.0f, 1.0f);
    v[3].color = shadowColor;

    UNLOCK_VB(m_ShadowVB);
}

void CMatrixMapVisual::BeforeDraw() {
    ++m_CurFrame;  // uniq number per ~500 days

    m_Camera->BeforeDraw();

    CPoint p(m_Cursor->GetPos());
    // ScreenToClient(g_Wnd,&p);
    m_Camera->CalcPickVector(p, m_Map->m_MouseDir);

    UpdateTraceObject();

    m_Map->m_Minimap.SetOutParams(D3DXVECTOR2(m_Camera->GetXYStrategy().x, m_Camera->GetXYStrategy().y));


    m_Map->GetVisibleCalculator()->CalcMapGroupVisibility();

    BeforeDrawLandscape();
    if (CTerSurface::IsSurfacesPresent())
        BeforeDrawLandscapeSurfaces();

    // build objects sort array

    CMatrixMapStatic::SortBegin();
    // CMatrixMapStatic::OnEndOfDraw(); // this will call OnOutScreen for all

    const auto viewMatrix = m_Camera->GetViewMatrix();
    for (auto visGroup : m_Map->GetVisibleCalculator()->GetVisibleGroupIterator()) {
        visGroup->SortObjects(viewMatrix);
        visGroup->BeforeDrawSurfaces();
    }

    for (int od = 0; od < m_AlwaysDrawStorage->GetObjectCount(); ++od) {
        auto object = m_AlwaysDrawStorage->Get(od);
        object->Sort(viewMatrix);
        if (object->GetObjectType() == OBJECT_TYPE_FLYER) {
            if (((CMatrixFlyer *)object)->CarryingRobot()) {
                ((CMatrixFlyer *)object)->GetCarryingRobot()->Sort(m_Camera->GetViewMatrix());
            }
        }
    }

    if (FLAG(m_Map->m_Flags, MMFLAG_NEEDRECALCTER)) {
        CMatrixMapStatic::SortEndRecalcTerainColor();
        RESETFLAG(m_Map->m_Flags, MMFLAG_NEEDRECALCTER);
    }

    if (FLAG(g_Config.m_DIFlags, DI_VISOBJ))
        m_DI->T(L"Visible objects", utils::format(L"%d", CMatrixMapStatic::GetVisObjCnt()).c_str());

    CMatrixMapStatic::SortEndBeforeDraw();

    // cannon for build before draw
    CMatrixSideUnit *player_side = m_Map->GetPlayerSide();
    if (player_side->m_CannonForBuild.m_Cannon) {
        if (g_IFaceList->m_InFocus == UNKNOWN) {
            player_side->m_CannonForBuild.m_Cannon->SetVisible(true);
            player_side->m_CannonForBuild.m_Cannon->BeforeDraw();
        }
        else {
            player_side->m_CannonForBuild.m_Cannon->SetVisible(false);
        }
    }

    m_Map->m_Minimap.BeforeDraw();

    CBillboard::BeforeDraw();
    for (PCMatrixEffect e = m_Map->m_EffectsFirst; e != NULL; e = e->m_Next) {
        e->BeforeDraw();
    }
    CMatrixProgressBar::BeforeDrawAll();
    m_Map->m_Water->BeforeDraw();

    g_IFaceList->BeforeRender();
}

void CMatrixMapVisual::Draw() {
    float fBias = -1.0f;

    if (FLAG(m_Map->m_Flags, MMFLAG_SKY_ON)) {
        DrawSky();
    }

    ASSERT_DX(g_D3DD->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP));
    ASSERT_DX(g_D3DD->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP));

        // one per frame
    ASSERT_DX(g_D3DD->SetTransform(D3DTS_VIEW, &m_Camera->GetViewMatrix()));
    ASSERT_DX(g_D3DD->SetTransform(D3DTS_PROJECTION, &m_Camera->GetProjMatrix()));

    // DrawObjects();
    // DrawLandscape();

    RESETFLAG(m_Map->m_Flags, MMFLAG_OBJECTS_DRAWN);
    if (FLAG(g_Flags, GFLAG_STENCILAVAILABLE) && g_Config.m_IzvratMS) {
        bool domask = CMultiSelection::DrawAllPass1Begin();
        DrawObjects();
        DrawLandscape();
        if (CTerSurface::IsSurfacesPresent())
            DrawLandscapeSurfaces();
        if (domask) {
            CMultiSelection::DrawAllPass2Begin();
            SETFLAG(m_Map->m_Flags, MMFLAG_OBJECTS_DRAWN);
            DrawObjects();

            // g_D3DD->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
            // CMatrixMapGroup::BeforeDrawAll();
            // DrawLandscape();
            CMultiSelection::DrawAllPassEnd();
        }
    }
    else {
        DrawObjects();
        DrawLandscape();
        if (CTerSurface::IsSurfacesPresent())
            DrawLandscapeSurfaces();
    }

    for (int i = 0; i < 4; i++) {
        ASSERT_DX(g_D3DD->SetSamplerState(i, D3DSAMP_MIPMAPLODBIAS, *((LPDWORD)(&(fBias)))));
    }

    CMatrixEffectLandscapeSpot::DrawAll();  // we should draw landscape spots immediately after draw landscape

    DrawWater();

    fBias = 0.0f;
    for (int i = 0; i < 4; i++) {
        ASSERT_DX(g_D3DD->SetSamplerState(i, D3DSAMP_MIPMAPLODBIAS, *((LPDWORD)(&(fBias)))));
    }
#ifndef RENDER_PROJ_SHADOWS_IN_STENCIL_PASS
    if (g_Config.m_ShowProjShadows) {
        DrawShadowsProjFast();
    }
#endif
    if (g_Config.m_ShowStencilShadows) {
        DrawShadows();
    }

    DrawEffects();

    for (int od = 0; od < m_AlwaysDrawStorage->GetObjectCount(); ++od) {
        auto object = m_AlwaysDrawStorage->Get(od);
        if (object->GetObjectType() == OBJECT_TYPE_FLYER) {
            ((CMatrixFlyer *)object)->DrawPropeller();
        }
    }

    CMatrixProgressBar::DrawAll();
    CMultiSelection::DrawAll();

    if (m_Map->IsPaused()) {
        ASSERT_DX(g_D3DD->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE));
        ASSERT_DX(g_D3DD->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE));
        ASSERT_DX(g_D3DD->SetRenderState(D3DRS_ZWRITEENABLE, FALSE));
        ASSERT_DX(g_D3DD->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA));
        ASSERT_DX(g_D3DD->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA));

        ASSERT_DX(g_D3DD->SetRenderState(D3DRS_TEXTUREFACTOR, 0x14000000 /* 0xffffffff*/));

        SetColorOpSelect(0, D3DTA_TFACTOR);
        SetAlphaOpSelect(0, D3DTA_TFACTOR);
        SetColorOpDisable(1);

        ASSERT_DX(g_D3DD->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE));
        ASSERT_DX(g_D3DD->SetStreamSource(0, GET_VB(m_ShadowVB), 0, sizeof(SShadowRectVertex)));
        ASSERT_DX(g_D3DD->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2));

        ASSERT_DX(g_D3DD->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE));
        ASSERT_DX(g_D3DD->SetRenderState(D3DRS_ZWRITEENABLE, TRUE));
        ASSERT_DX(g_D3DD->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE));
    }

#if (defined _DEBUG) && !(defined _RELDEBUG)
    CHelper::DrawAll();
#endif

    m_DI->Draw();  // debug info

    if (m_Map->m_DialogModeName) {
        if (wcscmp(m_Map->m_DialogModeName, TEMPLATE_DIALOG_MENU) == 0) {
            m_Map->m_DialogModeHints[0]->DrawNow();
        }
    }

    g_IFaceList->Render();
    CMatrixProgressBar::DrawAllClones();
    CMatrixHint::DrawAll();

    //_________________________________________________________________________________________________

    if (FLAG(m_Map->m_Flags, MMFLAG_TRANSITION)) {
        m_Map->m_Transition.Render();
    }

    //_________________________________________________________________________________________________

    m_Cursor->Draw();
}

void CMatrixMapVisual::DrawSky(void) {
    DTRACE();

    ASSERT_DX(g_Sampler.SetState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR));
    ASSERT_DX(g_Sampler.SetState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR));
    ASSERT_DX(g_Sampler.SetState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE));

    g_D3DD->SetRenderState(D3DRS_FOGENABLE, FALSE);
    g_D3DD->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    g_D3DD->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    float skyHeight = m_Map->GetVisibleCalculator()->GetSkyHeight();

    if (g_Config.m_SkyBox != 0 && m_SkyTex[0].tex) {
        ASSERT_DX(g_D3DD->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP));
        ASSERT_DX(g_D3DD->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP));

        SetColorOpSelect(0, D3DTA_TEXTURE);
        SetColorOpDisable(1);
        SetAlphaOpDisable(0);

        g_D3DD->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_D3DD->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
        g_D3DD->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

        D3DXMATRIX wo;
        D3DXMatrixPerspectiveFovLH(&wo, CAM_HFOV, float(g_ScreenX) * m_Camera->GetResYInversed(), 0.01f, 3);
        ASSERT_DX(g_D3DD->SetTransform(D3DTS_PROJECTION, &wo));

        ASSERT_DX(g_D3DD->SetTransform(D3DTS_WORLD, &m_Map->GetIdentityMatrix()));

        m_Camera->CalcSkyMatrix(wo);
        ASSERT_DX(g_D3DD->SetTransform(D3DTS_VIEW, &wo));
        // ASSERT_DX(g_D3DD->SetTransform( D3DTS_VIEW, &GetIdentityMatrix() ));

        SVert_V3_C_UV verts[4];
        verts[0].color = 0xFFFFFFFF;
        verts[1].color = 0xFFFFFFFF;
        verts[2].color = 0xFFFFFFFF;
        verts[3].color = 0xFFFFFFFF;

        float cut_dn = (200 + m_Camera->GetFrustumCenter().z) * 0.5f * INVERT(MAX_VIEW_DISTANCE) + 0.5f;
        float cut_up = 0.0f;

        float geo_dn = 2 * (1.0f - cut_dn) - 1;

        CInstDraw::BeginDraw(IDFVF_V3_C_UV);

        int tex = SKY_FORE;
        float v1 = (m_SkyTex[tex].v1 - m_SkyTex[tex].v0) * cut_dn + m_SkyTex[tex].v0;
        verts[0].p = D3DXVECTOR3(-1, 1, 1);
        verts[0].tu = m_SkyTex[tex].u0;
        verts[0].tv = m_SkyTex[tex].v0;
        verts[1].p = D3DXVECTOR3(-1, geo_dn, 1);
        verts[1].tu = m_SkyTex[tex].u0;
        verts[1].tv = v1;
        verts[2].p = D3DXVECTOR3(1, 1, 1);
        verts[2].tu = m_SkyTex[tex].u1;
        verts[2].tv = m_SkyTex[tex].v0;
        verts[3].p = D3DXVECTOR3(1, geo_dn, 1);
        verts[3].tu = m_SkyTex[tex].u1;
        verts[3].tv = v1;

        CInstDraw::AddVerts(verts, m_SkyTex[tex].tex);

        tex = SKY_RITE;
        v1 = (m_SkyTex[tex].v1 - m_SkyTex[tex].v0) * cut_dn + m_SkyTex[tex].v0;
        verts[0].p = D3DXVECTOR3(1, 1, 1);
        verts[0].tu = m_SkyTex[tex].u0;
        verts[0].tv = m_SkyTex[tex].v0;
        verts[1].p = D3DXVECTOR3(1, geo_dn, 1);
        verts[1].tu = m_SkyTex[tex].u0;
        verts[1].tv = v1;
        verts[2].p = D3DXVECTOR3(1, 1, -1);
        verts[2].tu = m_SkyTex[tex].u1;
        verts[2].tv = m_SkyTex[tex].v0;
        verts[3].p = D3DXVECTOR3(1, geo_dn, -1);
        verts[3].tu = m_SkyTex[tex].u1;
        verts[3].tv = v1;

        CInstDraw::AddVerts(verts, m_SkyTex[tex].tex);

        tex = SKY_LEFT;
        v1 = (m_SkyTex[tex].v1 - m_SkyTex[tex].v0) * cut_dn + m_SkyTex[tex].v0;
        verts[0].p = D3DXVECTOR3(-1, 1, -1);
        verts[0].tu = m_SkyTex[tex].u0;
        verts[0].tv = m_SkyTex[tex].v0;
        verts[1].p = D3DXVECTOR3(-1, geo_dn, -1);
        verts[1].tu = m_SkyTex[tex].u0;
        verts[1].tv = v1;
        verts[2].p = D3DXVECTOR3(-1, 1, 1);
        verts[2].tu = m_SkyTex[tex].u1;
        verts[2].tv = m_SkyTex[tex].v0;
        verts[3].p = D3DXVECTOR3(-1, geo_dn, 1);
        verts[3].tu = m_SkyTex[tex].u1;
        verts[3].tv = v1;

        CInstDraw::AddVerts(verts, m_SkyTex[tex].tex);

        tex = SKY_BACK;
        v1 = (m_SkyTex[tex].v1 - m_SkyTex[tex].v0) * cut_dn + m_SkyTex[tex].v0;
        verts[0].p = D3DXVECTOR3(1, 1, -1);
        verts[0].tu = m_SkyTex[tex].u0;
        verts[0].tv = m_SkyTex[tex].v0;
        verts[1].p = D3DXVECTOR3(1, geo_dn, -1);
        verts[1].tu = m_SkyTex[tex].u0;
        verts[1].tv = v1;
        verts[2].p = D3DXVECTOR3(-1, 1, -1);
        verts[2].tu = m_SkyTex[tex].u1;
        verts[2].tv = m_SkyTex[tex].v0;
        verts[3].p = D3DXVECTOR3(-1, geo_dn, -1);
        verts[3].tu = m_SkyTex[tex].u1;
        verts[3].tv = v1;

        CInstDraw::AddVerts(verts, m_SkyTex[tex].tex);

        CInstDraw::ActualDraw();

        g_D3DD->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

        float SH1 = float(g_ScreenY * 0.270416666666667);
        float SH2 = float(g_ScreenY * 0.07);

        g_D3DD->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

        DWORD m_SkyColorUp = m_SkyColor & 0x00FFFFFF;
        SVert_V4_C v[6];
        v[0].p = D3DXVECTOR4(0, skyHeight - SH1, 0.0f, 1.0f);
        v[0].col = m_SkyColorUp;
        v[1].p = D3DXVECTOR4(float(g_ScreenX), skyHeight - SH1, 0.0f, 1.0f);
        v[1].col = m_SkyColorUp;
        v[2].p = D3DXVECTOR4(0, skyHeight - SH2, 0.0f, 1.0f);
        v[2].col = m_SkyColor;
        v[3].p = D3DXVECTOR4(float(g_ScreenX), skyHeight - SH2, 0.0f, 1.0f);
        v[3].col = m_SkyColor;
        v[4].p = D3DXVECTOR4(0, skyHeight, 0.0f, 1.0f);
        v[4].col = m_SkyColor;
        v[5].p = D3DXVECTOR4(float(g_ScreenX), skyHeight, 0.0f, 1.0f);
        v[5].col = m_SkyColor;

        SetAlphaOpSelect(0, D3DTA_DIFFUSE);

        SetColorOpSelect(0, D3DTA_DIFFUSE);
        SetColorOpDisable(1);

        ASSERT_DX(g_D3DD->SetRenderState(D3DRS_ZWRITEENABLE, FALSE));
        ASSERT_DX(g_D3DD->SetRenderState(D3DRS_ZENABLE, FALSE));

        CInstDraw::BeginDraw(IDFVF_V4_C);
        CInstDraw::AddVerts(v, NULL);
        CInstDraw::AddVerts(v + 2, NULL);
        CInstDraw::ActualDraw();
    }
    else {
        // do not draw skybox

        float SH1 = float(g_ScreenY * 0.270416666666667);
        float SH2 = float(g_ScreenY * 0.07);

        DWORD m_SkyColorUp = 0x00000000;
        SVert_V4_C v[6];
        v[0].p = D3DXVECTOR4(0, skyHeight - SH1, 0.0f, 1.0f);
        v[0].col = m_SkyColorUp;
        v[1].p = D3DXVECTOR4(float(g_ScreenX), skyHeight - SH1, 0.0f, 1.0f);
        v[1].col = m_SkyColorUp;

        v[2].p = D3DXVECTOR4(0, skyHeight - SH2, 0.0f, 1.0f);
        v[2].col = m_SkyColor;
        v[3].p = D3DXVECTOR4(float(g_ScreenX), skyHeight - SH2, 0.0f, 1.0f);
        v[3].col = m_SkyColor;

        v[4].p = D3DXVECTOR4(0, skyHeight, 0.0f, 1.0f);
        v[4].col = m_SkyColor;
        v[5].p = D3DXVECTOR4(float(g_ScreenX), skyHeight, 0.0f, 1.0f);
        v[5].col = m_SkyColor;

        if (v[0].p.y > 0) {
            v[0].p.y = 0;
            v[1].p.y = 0;
        }

        SetAlphaOpDisable(0);

        SetColorOpSelect(0, D3DTA_DIFFUSE);
        SetColorOpDisable(1);

        ASSERT_DX(g_D3DD->SetRenderState(D3DRS_ZWRITEENABLE, FALSE));
        ASSERT_DX(g_D3DD->SetRenderState(D3DRS_ZENABLE, FALSE));

        CInstDraw::BeginDraw(IDFVF_V4_C);
        CInstDraw::AddVerts(v, NULL);
        CInstDraw::AddVerts(v + 2, NULL);
        CInstDraw::ActualDraw();

        // D3DRECT r;
        // r.x1 = 0;
        // r.y1 = Float2Int(g_MatrixMap->skyHeight);
        // r.x2 = g_ScreenX;
        // r.y2 = r.y1 + 20;
        // ASSERT_DX(g_D3DD->Clear( 1, &r, D3DCLEAR_TARGET, g_MatrixMap->m_SkyColor, 1.0f, 0 ));
    }

    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_ZWRITEENABLE, TRUE));
    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_ZENABLE, TRUE));
    g_D3DD->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void CMatrixMapVisual::DrawFog() {
    // enable fogging

    if (FLAG(m_Map->m_Flags, MMFLAG_FOG_ON)) {
        float Start = float(MAX_VIEW_DISTANCE * FOG_NEAR_K);  // For linear mode
        float End = float(MAX_VIEW_DISTANCE * FOG_FAR_K);
        if (g_D3DDCaps.RasterCaps & D3DPRASTERCAPS_WFOG) {}
        else {
            Start /= MAX_VIEW_DISTANCE;
            End /= MAX_VIEW_DISTANCE;
        }
        // Enable fog blending.
        g_D3DD->SetRenderState(D3DRS_FOGENABLE, TRUE);

        // Set the fog color.
        g_D3DD->SetRenderState(D3DRS_FOGCOLOR, m_SkyColor);

        g_D3DD->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);
        g_D3DD->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
        g_D3DD->SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&Start));
        g_D3DD->SetRenderState(D3DRS_FOGEND, *(DWORD *)(&End));
    }
    else {
        g_D3DD->SetRenderState(D3DRS_FOGENABLE, FALSE);
    }
}

void CMatrixMapVisual::UpdateTraceObject() {
    m_Map->m_TraceStopObj = m_Map->Trace(&m_Map->m_TraceStopPos, m_Camera->GetFrustumCenter(),
                                         m_Camera->GetFrustumCenter() + (m_Map->m_MouseDir * 10000.0f), TRACE_ALL,
                                         g_MatrixMap->GetPlayerSide()->GetArcadedObject());

    // TODO Hack
    auto m_TraceStopObj = m_Map->m_TraceStopObj;

    if (IS_TRACE_STOP_OBJECT(m_Map->m_TraceStopObj)) {
        if (m_TraceStopObj->GetObjectType() == OBJECT_TYPE_BUILDING) {
            ((CMatrixBuilding *)m_TraceStopObj)->ShowHitpoint();
        }
        else if (m_TraceStopObj->GetObjectType() == OBJECT_TYPE_ROBOTAI) {
            ((CMatrixRobotAI *)m_TraceStopObj)->ShowHitpoint();
        }
        else if (m_TraceStopObj->GetObjectType() == OBJECT_TYPE_CANNON) {
            ((CMatrixCannon *)m_TraceStopObj)->ShowHitpoint();

        }
        else if (m_TraceStopObj->GetObjectType() == OBJECT_TYPE_FLYER) {
            ((CMatrixFlyer *)m_TraceStopObj)->ShowHitpoint();
        }

#ifdef _DEBUG
        if (m_TraceStopObj->GetObjectType() == OBJECT_TYPE_MAPOBJECT)
            m_DI->T(L"Under cursor", L"Mesh", 1000);
        else if (m_TraceStopObj->GetObjectType() == OBJECT_TYPE_ROBOTAI) {
            m_DI->T(L"Under cursor",
                   utils::format(L"Robot %x   S%d T%d G%d", DWORD(m_TraceStopObj), m_TraceStopObj->GetSide(),
                                 ((CMatrixRobotAI *)m_TraceStopObj)->GetTeam(),
                                 ((CMatrixRobotAI *)m_TraceStopObj)->GetGroupLogic())
                           .c_str(),
                   1000);
        }
        else if (m_TraceStopObj->GetObjectType() == OBJECT_TYPE_CANNON)
            m_DI->T(L"Under cursor", L"Cannon", 1000);
        else if (m_TraceStopObj->GetObjectType() == OBJECT_TYPE_BUILDING) {
            m_DI->T(L"Under cursor",
                   utils::format(L"Building: 0x%X", reinterpret_cast<dword>((void *)m_TraceStopObj)).c_str(), 1000);
        }
        else if (m_TraceStopObj->GetObjectType() == OBJECT_TYPE_FLYER)
            m_DI->T(L"Under cursor", L"Flyer", 1000);
#endif
    }
}


void CMatrixMapVisual::BeforeDrawLandscape(bool all) {
    DTRACE();

    CMatrixMapGroup::BeforeDrawAll();
    m_Map->GetMacroTexture().Prepare();

    if (all) {
        for (auto group : m_Map->GetVisibleCalculator()->GetGroupsIterator()) {
            if (group) {
                group->BeforeDraw();
            }
        }
    }
    else {
        for (auto group : m_Map->GetVisibleCalculator()->GetVisibleGroupIterator()) {
            group->BeforeDraw();
        }
    }
}

void CMatrixMapVisual::BeforeDrawLandscapeSurfaces(bool all) {
    DTRACE();

    CTerSurface::BeforeDrawAll();

    m_Map->GetMacroTexture().Prepare();

    if (all) {
        for (auto group : m_Map->GetVisibleCalculator()->GetGroupsIterator()) {
            if (group) {
                group->BeforeDrawSurfaces();
            }
        }
    }
    else {
        for (auto group : m_Map->GetVisibleCalculator()->GetVisibleGroupIterator()) {
            group->BeforeDrawSurfaces();
        }
    }
}


void CMatrixMapVisual::DrawLandscapeSurfaces(bool all) {
    DTRACE();

    for (int i = 0; i < 8; i++) {
        ASSERT_DX(g_D3DD->SetSamplerState(i, D3DSAMP_MIPMAPLODBIAS, *((LPDWORD)(&m_BiasTer))));
        ASSERT_DX(g_Sampler.SetState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR));
        ASSERT_DX(g_Sampler.SetState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR));
        ASSERT_DX(g_Sampler.SetState(i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR));
    }

    // ASSERT_DX(g_D3DD->SetTransform(D3DTS_WORLD,&GetIdentityMatrix()));

    // int cnt;
    // CMatrixMapGroup * * md;

    //   if (all)
    //   {
    //    cnt = m_GroupSize.x*m_GroupSize.y;
    //    md = m_Group;
    //   } else
    //   {
    //    cnt = m_VisibleGroupsCount;
    //    md = m_VisibleGroups;
    //   }
    // while(cnt>0)
    //   {
    //       if (*md) (*md)->DrawSurfaces();
    //       md++;
    //	cnt--;
    //   }

    CTerSurface::DrawAll();
}

void CMatrixMapVisual::DrawLandscape(bool all) {
    DTRACE();

#ifdef _DEBUG
    CDText::T("land_dp_calls", CMatrixMapGroup::m_DPCalls);
    CMatrixMapGroup::m_DPCalls = 0;
#endif

    ASSERT_DX(g_D3DD->SetSamplerState(0, D3DSAMP_MIPMAPLODBIAS, *((LPDWORD)(&m_BiasTer))));
    ASSERT_DX(g_Sampler.SetState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR));
    ASSERT_DX(g_Sampler.SetState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT));
    ASSERT_DX(g_Sampler.SetState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR));

    // ASSERT_DX(g_D3DD->SetTransform(D3DTS_WORLD,&GetIdentityMatrix()));

    ASSERT_DX(g_D3DD->SetFVF(MATRIX_MAP_BOTTOM_VERTEX_FORMAT));
    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_LIGHTING, FALSE));

    auto iter = all ? GetVisibleCalculator()->GetGroupsIterator() : GetVisibleCalculator()->GetVisibleGroupIterator();

    for (auto group : iter) {
        if (group) {
            group->Draw();
        }
    }
}

void CMatrixMapVisual::DrawObjects(void) {
    DTRACE();

    // ASSERT_DX(g_D3DD->SetTextureStageState(1,D3DTSS_TEXCOORDINDEX,0));
    // ASSERT_DX(g_D3DD->SetTextureStageState(2,D3DTSS_COLOROP,D3DTOP_DISABLE));

    CVectorObject::DrawBegin();

    // if (m_KeyScan != KEY_L || m_KeyDown == false)

    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_LIGHTING, TRUE));

    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_AMBIENT, m_AmbientColorObj));
    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_TEXTUREFACTOR, 0xFFFFFFFF));

    //   ASSERT_DX(g_D3DD->SetRenderState( D3DRS_TEXTUREFACTOR, m_AmbientColorObj ));
    // ASSERT_DX(g_D3DD->SetRenderState( D3DRS_AMBIENT, 0xFFFFFFFF));

    for (int i = 0; i < 8; i++) {
        ASSERT_DX(g_D3DD->SetSamplerState(i, D3DSAMP_MIPMAPLODBIAS, *((LPDWORD)(&m_BiasRobots))));
        ASSERT_DX(g_Sampler.SetState(i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR));
        ASSERT_DX(g_Sampler.SetState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR));
        ASSERT_DX(g_Sampler.SetState(i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR));
    }

    // draw all objects
    CMatrixMapStatic::SortEndDraw();

    // draw cannon for build

    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_LIGHTING, FALSE));

    CMatrixSideUnit *player_side = m_Map->GetPlayerSide();
    if (player_side->m_CannonForBuild.m_Cannon) {
        if (player_side->m_CannonForBuild.m_Cannon->IsVisible())
            player_side->m_CannonForBuild.m_Cannon->Draw();
    }

    CVectorObject::DrawEnd();
}

void CMatrixMapVisual::DrawWater() {
    DTRACE();

    if (!m_Map->m_Water->IsReadyForDraw())
        return;

    // g_D3DD->SetRenderState( D3DRS_NORMALIZENORMALS,  TRUE );

    // g_D3DD->SetMaterial(&mtrl );
    // g_D3DD->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);
    // g_D3DD->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR1);

    ASSERT_DX(g_Sampler.SetState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR));
    ASSERT_DX(g_Sampler.SetState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR));
    ASSERT_DX(g_Sampler.SetState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR));

    for (int i = 0; i < 4; i++) {
        ASSERT_DX(g_D3DD->SetSamplerState(i, D3DSAMP_MIPMAPLODBIAS, *((LPDWORD)(&m_BiasWater))));

        ASSERT_DX(g_D3DD->SetSamplerState(i, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP));
        ASSERT_DX(g_D3DD->SetSamplerState(i, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP));
    }

    D3DXMATRIX m = this->GetIdentityMatrix();

    m._11 *= GLOBAL_SCALE * (float)(MAP_GROUP_SIZE) / (float)(WATER_SIZE);
    m._22 *= GLOBAL_SCALE * (float)(MAP_GROUP_SIZE) / (float)(WATER_SIZE);
    m._33 *= GLOBAL_SCALE * (float)(MAP_GROUP_SIZE) / (float)(WATER_SIZE);
    m._43 = WATER_LEVEL;

    int curpass;

    for (curpass = 0; curpass < g_Render->m_WaterPassAlpha; ++curpass) {
        g_Render->m_WaterAlpha(m_Map->m_Water->m_WaterTex1, m_Map->m_Water->m_WaterTex2, curpass);

        for (CMatrixMapGroup *group : GetVisibleCalculator()->GetVisibleGroupIterator()) {
            if (group->HasWater()) {
                if (curpass == 0)
                    ASSERT_DX(g_D3DD->SetTexture(0, group->GetWaterAlpha()->Tex()));

                const D3DXVECTOR2 &p = group->GetPos0();
                m._41 = p.x;
                m._42 = p.y;
                m_Map->m_Water->Draw(m);
            }
        }
    }

    g_Render->m_WaterClearAlpha();

    for (curpass = 0; curpass < g_Render->m_WaterPassSolid; ++curpass) {
        g_Render->m_WaterSolid(m_Map->m_Water->m_WaterTex1, m_Map->m_Water->m_WaterTex2, curpass);

        for (const auto &item : GetVisibleCalculator()->GetVisWater()) {
            m._41 = item.x;
            m._42 = item.y;
            m_Map->m_Water->Draw(m);
        }
    }

    g_Render->m_WaterClearSolid();

    // g_D3DD->SetRenderState( D3DRS_NORMALIZENORMALS,  FALSE );

    if (SInshorewave::m_Tex) {
        SInshorewave::DrawBegin();

        for (auto group : GetVisibleCalculator()->GetVisibleGroupIterator()) {
            if (group->HasWater()) {
                group->DrawInshorewaves();
            }
        }

        SInshorewave::DrawEnd();
    }
}

void CMatrixMapVisual::DrawShadowsProjFast(void) {
    DTRACE();

    CVOShadowProj::BeforeRenderAll();

    // g_D3DD->SetRenderState(D3DRS_ALPHATESTENABLE,		TRUE);
    g_D3DD->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA));
    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA));

    g_D3DD->SetRenderState(D3DRS_TEXTUREFACTOR, m_ShadowColor);

    SetColorOpSelect(0, D3DTA_TFACTOR);
    SetAlphaOpAnyOrder(0, D3DTOP_MODULATE, D3DTA_TEXTURE, D3DTA_TFACTOR);
    SetColorOpDisable(1);

    ASSERT_DX(g_Sampler.SetState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR));
    ASSERT_DX(g_Sampler.SetState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR));
    ASSERT_DX(g_Sampler.SetState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE));

    g_D3DD->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

    ASSERT_DX(g_D3DD->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP));
    ASSERT_DX(g_D3DD->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP));

    CMatrixMapStatic::SortEndDrawShadowProj();

    for (auto group : GetVisibleCalculator()->GetVisibleGroupIterator()) {
        group->DrawShadowProj();
    }

    // g_D3DD->SetRenderState(D3DRS_ALPHATESTENABLE,		FALSE);
    g_D3DD->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    g_D3DD->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

    ASSERT_DX(g_D3DD->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP));
    ASSERT_DX(g_D3DD->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP));
}

void CMatrixMapVisual::DrawShadows(void) {
    DTRACE();

    CVOShadowStencil::BeforeRenderAll();

    ASSERT_DX(g_D3DD->SetTransform(D3DTS_WORLD, &GetIdentityMatrix()));

    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_ZWRITEENABLE, FALSE));
    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_STENCILENABLE, TRUE));
    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT));

    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS));
    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP));
    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP));

    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_STENCILREF, 0x1));
    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_STENCILMASK, 0xffffffff));
    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff));
    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR));

    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE));
    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO));
    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE));

    if (g_D3DDCaps.StencilCaps & D3DSTENCILCAPS_TWOSIDED) {
        ASSERT_DX(g_D3DD->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, TRUE));
        ASSERT_DX(g_D3DD->SetRenderState(D3DRS_CCW_STENCILFUNC, D3DCMP_ALWAYS));
        ASSERT_DX(g_D3DD->SetRenderState(D3DRS_CCW_STENCILZFAIL, D3DSTENCILOP_KEEP));
        ASSERT_DX(g_D3DD->SetRenderState(D3DRS_CCW_STENCILFAIL, D3DSTENCILOP_KEEP));
        ASSERT_DX(g_D3DD->SetRenderState(D3DRS_CCW_STENCILPASS, D3DSTENCILOP_DECR));

        ASSERT_DX(g_D3DD->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));

        CMatrixMapStatic::SortEndDrawShadowStencil();

        ASSERT_DX(g_D3DD->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, FALSE));
    }
    else {
        CMatrixMapStatic::SortEndDrawShadowStencil();

        ASSERT_DX(g_D3DD->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW));
        ASSERT_DX(g_D3DD->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_DECR));

        CMatrixMapStatic::SortEndDrawShadowStencil();
    }
    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW));
    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR));

#ifdef RENDER_PROJ_SHADOWS_IN_STENCIL_PASS
    // Shadow proj

    CVOShadowProj::BeforeRenderAll();

    ASSERT_DX(g_D3DD->SetTransform(D3DTS_WORLD, &GetIdentityMatrix()));

    ASSERT_DX(g_D3DD->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP));
    ASSERT_DX(g_D3DD->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP));

    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE));
    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL));
    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_ALPHAREF, 0x8));

    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS));
    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP));
    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP));

    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_STENCILREF, 0x1));
    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_STENCILMASK, 0xffffffff));
    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff));
    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE));

    SetColorOpSelect(0, D3DTA_TEXTURE);
    SetAlphaOpSelect(0, D3DTA_TEXTURE);
    SetColorOpDisable(1);

    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_COLORWRITEENABLE, 0x0));

    CMatrixMapStatic::SortEndDrawShadowProj();

    for (CMatrixMapGroup *group : GetVisibleCalculator()->GetVisibleGroupIterator()) {
        group->DrawShadowProj();
    }

    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_COLORWRITEENABLE, 0xF));

    ASSERT_DX(g_D3DD->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP));
    ASSERT_DX(g_D3DD->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP));

    ASSERT_DX(g_D3DD->SetTexture(0, NULL));
#endif

    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_DECR));

    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD));
    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_STENCILENABLE, FALSE));

    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE));
    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_ZWRITEENABLE, FALSE));

    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_STENCILENABLE, TRUE));
    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA));
    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA));

    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_TEXTUREFACTOR, 0xC0C0C0C0 /* 0xffffffff*/));

    SetColorOpAnyOrder(0, D3DTOP_MODULATE, D3DTA_DIFFUSE, D3DTA_TFACTOR);
    SetAlphaOpAnyOrder(0, D3DTOP_MODULATE, D3DTA_DIFFUSE, D3DTA_TFACTOR);
    SetColorOpDisable(1);

    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_STENCILREF, 0x1));
    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_LESSEQUAL));
    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP));

    ASSERT_DX(g_D3DD->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE));
    ASSERT_DX(g_D3DD->SetStreamSource(0, GET_VB(m_ShadowVB), 0, sizeof(SShadowRectVertex)));
    ASSERT_DX(g_D3DD->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2));

    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE));
    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_ZWRITEENABLE, TRUE));
    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_STENCILENABLE, FALSE));
    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE));
    ASSERT_DX(g_D3DD->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE));
}

void CMatrixMapVisual::DrawEffects(void) {
    DTRACE();

    CMatrixEffect::DrawBegin();

    // CSortable::SortBegin();

    for (PCMatrixEffect e = m_Map->m_EffectsFirst; e != NULL; e = e->m_Next) {
        e->Draw();
    }

    ASSERT_DX(g_D3DD->SetTransform(D3DTS_WORLD, &GetIdentityMatrix()));
    CBillboard::SortEndDraw(m_Camera->GetViewInversedMatrix(), m_Camera->GetFrustumCenter());
    CMatrixEffect::DrawEnd();
}
