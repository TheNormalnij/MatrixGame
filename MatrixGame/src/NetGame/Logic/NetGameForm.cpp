// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include <string>

#include "NetGameForm.h"
#include "MatrixMap.hpp"
#include "MatrixRobot.hpp"
#include "MatrixFlyer.hpp"
#include "Effects/MatrixEffect.hpp"
#include "Effects/MatrixEffectSelection.hpp"
#include "Effects/MatrixEffectPointLight.hpp"
#include "Effects/MatrixEffectPath.hpp"
#include "Interface/CInterface.h"
#include "MatrixMultiSelection.hpp"
#include "MatrixMapStatic.hpp"
#include "Interface/CIFaceMenu.h"
#include "MatrixGameDll.hpp"
#include "MatrixInstantDraw.hpp"
#include "Interface/MatrixHint.hpp"
#include "MatrixObjectCannon.hpp"
#include "Interface/CCounter.h"
#include "MatrixGamePathUtils.hpp"

//#include <time.h>
//#include <sys/timeb.h>
//#include "stdio.h"
//
//#include <utils.hpp>

CNetGameForm::CNetGameForm() : CForm() {
    DTRACE();
    m_Name = L"MatrixNetGame";

    m_LastWorldX = 0;
    m_LastWorldY = 0;
    m_Action = 0;
}

CNetGameForm::~CNetGameForm() {
    DTRACE();
}

void CNetGameForm::Enter(void) {
    DTRACE();
    S3D_Default();
    D3DMATERIAL9 mtrl;
    ZeroMemory(&mtrl, sizeof(D3DMATERIAL9));
    mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
    mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
    mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
    mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
    mtrl.Specular.r = 0.5f;
    mtrl.Specular.g = 0.5f;
    mtrl.Specular.b = 0.5f;
    mtrl.Specular.a = 0.5f;
    g_D3DD->SetMaterial(&mtrl);
    g_D3DD->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);

    D3DXVECTOR3 vecDir;
    D3DLIGHT9 light;
    ZeroMemory(&light, sizeof(D3DLIGHT9));
    light.Type = D3DLIGHT_DIRECTIONAL;  // D3DLIGHT_POINT;//D3DLIGHT_DIRECTIONAL;
    light.Diffuse.r = GetColorR(g_MatrixMap->m_LightMainColorObj);
    light.Diffuse.g = GetColorG(g_MatrixMap->m_LightMainColorObj);
    light.Diffuse.b = GetColorB(g_MatrixMap->m_LightMainColorObj);
    light.Ambient.r = 0.0f;
    light.Ambient.g = 0.0f;
    light.Ambient.b = 0.0f;
    light.Specular.r = GetColorR(g_MatrixMap->m_LightMainColorObj);
    light.Specular.g = GetColorG(g_MatrixMap->m_LightMainColorObj);
    light.Specular.b = GetColorB(g_MatrixMap->m_LightMainColorObj);
    // light.Range       = 0;
    light.Direction = g_MatrixMap->m_LightMain;
    //	light.Direction=D3DXVECTOR3(250.0f,-50.0f,-250.0f);
    //	D3DXVec3Normalize((D3DXVECTOR3 *)(&(light.Direction)),(D3DXVECTOR3 *)(&(light.Direction)));
    ASSERT_DX(g_D3DD->SetLight(0, &light));
    ASSERT_DX(g_D3DD->LightEnable(0, TRUE));

    g_MatrixMap->m_Cursor.SetVisible(true);
}

void CNetGameForm::Leave(void) {
    DTRACE();
}

void CNetGameForm::Draw(void) {
    DTRACE();

    if (!FLAG(g_MatrixMap->m_Flags, MMFLAG_VIDEO_RESOURCES_READY))
        return;

    CInstDraw::DrawFrameBegin();

    g_MatrixMap->BeforeDraw();

    // ASSERT_DX(g_D3DD->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
    // D3DCOLOR_XRGB(255,0,0), 1.0f, 0 ));

    if (FLAG(g_Flags, GFLAG_STENCILAVAILABLE)) {
        ASSERT_DX(g_D3DD->Clear(0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 1.0f, 0));

        if (CInterface::ClearRects_GetCount())
            ASSERT_DX(g_D3DD->Clear(CInterface::ClearRects_GetCount(), CInterface::ClearRects_Get(),
                                    D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(255, 0, 255), 0.0f, 0));
    }
    else {
        ASSERT_DX(g_D3DD->Clear(0, NULL, D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 0, 255), 1.0f, 0));

        if (CInterface::ClearRects_GetCount())
            ASSERT_DX(g_D3DD->Clear(CInterface::ClearRects_GetCount(), CInterface::ClearRects_Get(), D3DCLEAR_ZBUFFER,
                                    D3DCOLOR_XRGB(255, 0, 255), 0.0f, 0));
    }

    ASSERT_DX(g_D3DD->BeginScene());

    g_MatrixMap->Draw();

    ASSERT_DX(g_D3DD->EndScene());

    // SETFLAG(g_Flags, GFLAG_PRESENT_REQUIRED);
    static int current_delay = 0;

    static DWORD last_time = 0;
    DWORD ctime = timeGetTime();
    int step1 = (last_time <= ctime) ? (ctime - last_time) : (0xFFFFFFFF - last_time + ctime);
    last_time = ctime;

    float cfps = 1000.0f / float(step1);

    if (cfps > float(g_MaxFPS)) {
        ++current_delay;
    }
    else {
        --current_delay;
        if (current_delay < 0)
            current_delay = 0;
    }

    Sleep(current_delay);

    ASSERT_DX(g_D3DD->Present(NULL, NULL, NULL, NULL));
}

void CNetGameForm::Takt(int step) {
    DTRACE();

    if (g_MatrixMap->CheckLostDevice())
        return;

    g_MatrixMap->Takt(step);

    CPoint mp = g_MatrixMap->m_Cursor.GetPos();

    if (!g_MatrixMap->GetPlayerSide()->IsArcadeMode()) {
        if (mp.x >= 0 && mp.x < g_ScreenX && mp.y >= 0 && mp.y < g_ScreenY) {
            if (mp.x < MOUSE_BORDER)
                g_MatrixMap->m_Camera.MoveLeft();
            if (mp.x > (g_ScreenX - MOUSE_BORDER))
                g_MatrixMap->m_Camera.MoveRight();
            if (mp.y < MOUSE_BORDER)
                g_MatrixMap->m_Camera.MoveUp();
            if (mp.y > (g_ScreenY - MOUSE_BORDER))
                g_MatrixMap->m_Camera.MoveDown();
        }
    }

    if (g_MatrixMap->m_Console.IsActive())
        return;

    if (!g_MatrixMap->GetPlayerSide()->IsArcadeMode()) {
        if (((GetAsyncKeyState(g_Config.m_KeyActions[KA_SCROLL_LEFT]) & 0x8000) == 0x8000) ||
            ((GetAsyncKeyState(g_Config.m_KeyActions[KA_SCROLL_LEFT_ALT]) & 0x8000) == 0x8000)) {
            g_MatrixMap->m_Camera.MoveLeft();
        }
        if (((GetAsyncKeyState(g_Config.m_KeyActions[KA_SCROLL_RIGHT]) & 0x8000) == 0x8000) ||
            ((GetAsyncKeyState(g_Config.m_KeyActions[KA_SCROLL_RIGHT_ALT]) & 0x8000) == 0x8000)) {
            g_MatrixMap->m_Camera.MoveRight();
        }
        if (((GetAsyncKeyState(g_Config.m_KeyActions[KA_SCROLL_UP]) & 0x8000) == 0x8000) ||
            ((GetAsyncKeyState(g_Config.m_KeyActions[KA_SCROLL_UP_ALT]) & 0x8000) == 0x8000)) {
            g_MatrixMap->m_Camera.MoveUp();
        }
        if (((GetAsyncKeyState(g_Config.m_KeyActions[KA_SCROLL_DOWN]) & 0x8000) == 0x8000) ||
            ((GetAsyncKeyState(g_Config.m_KeyActions[KA_SCROLL_DOWN_ALT]) & 0x8000) == 0x8000)) {
            g_MatrixMap->m_Camera.MoveDown();
        }
        if (((GetAsyncKeyState(g_Config.m_KeyActions[KA_ROTATE_LEFT]) & 0x8000) == 0x8000) ||
            ((GetAsyncKeyState(g_Config.m_KeyActions[KA_ROTATE_LEFT_ALT]) & 0x8000) == 0x8000)) {
            g_MatrixMap->m_Camera.RotLeft();
        }
        if (((GetAsyncKeyState(g_Config.m_KeyActions[KA_ROTATE_RIGHT]) & 0x8000) == 0x8000) ||
            ((GetAsyncKeyState(g_Config.m_KeyActions[KA_ROTATE_RIGHT_ALT]) & 0x8000) == 0x8000)) {
            g_MatrixMap->m_Camera.RotRight();
        }
    }

    {
        if (((GetAsyncKeyState(g_Config.m_KeyActions[KA_ROTATE_UP]) & 0x8000) == 0x8000)) {
            g_MatrixMap->m_Camera.RotUp();
        }
        if (((GetAsyncKeyState(g_Config.m_KeyActions[KA_ROTATE_DOWN]) & 0x8000) == 0x8000)) {
            g_MatrixMap->m_Camera.RotDown();
        }
    }

    if (GetAsyncKeyState(/*VK_SNAPSHOT*/ VK_F9) != 0) {
        const std::wstring screenshots_dir{PathToOutputFiles(FOLDER_NAME_SCREENSHOTS)};

        static uint16_t index = 0;
        index++;

        CreateDirectoryW(screenshots_dir.c_str(), NULL);

        time_t cur_time = time(NULL);
        struct tm tstruct = *localtime(&cur_time);
        wchar_t time_str[20];
        wcsftime(time_str, sizeof(time_str), L"%Y-%m-%d-%H-%M-%S", &tstruct);

        std::wstring filename =
                utils::format(L"%ls\\%ls-%ls-%03d.png", screenshots_dir.c_str(), FILE_NAME_SCREENSHOT, time_str, index);

        DeleteFileW(filename.c_str());

        if (!g_D3Dpp.Windowed) {
            IDirect3DSurface9 *pTargetSurface = NULL;
            HRESULT hr = D3D_OK;

            if (!g_D3Dpp.MultiSampleType)
                hr = g_D3DD->GetRenderTarget(0, &pTargetSurface);

            if (hr == D3D_OK) {
                D3DSURFACE_DESC desc;

                if (!g_D3Dpp.MultiSampleType) {
                    hr = pTargetSurface->GetDesc(&desc);
                }
                else {
                    desc.Width = g_ScreenX;
                    desc.Height = g_ScreenY;
                    desc.Format = D3DFMT_A8R8G8B8;
                }
                if (hr == D3D_OK) {
                    IDirect3DSurface9 *pSurface = NULL;
                    hr = g_D3DD->CreateOffscreenPlainSurface(desc.Width, desc.Height, desc.Format, D3DPOOL_SYSTEMMEM,
                                                             &pSurface, NULL);
                    if (hr == D3D_OK) {
                        if (!g_D3Dpp.MultiSampleType) {
                            hr = g_D3DD->GetRenderTargetData(pTargetSurface, pSurface);
                        }
                        else {
                            hr = g_D3DD->GetFrontBufferData(0, pSurface);
                        }
                        if (hr == D3D_OK) {
                            D3DLOCKED_RECT lockrect;
                            hr = pSurface->LockRect(&lockrect, NULL, 0);
                            if (hr == D3D_OK) {
                                CBitmap bm;
                                bm.CreateRGB(desc.Width, desc.Height);

                                for (UINT y = 0; y < desc.Height; y++) {
                                    unsigned char *buf_src = (unsigned char *)lockrect.pBits + lockrect.Pitch * y;
                                    unsigned char *buf_des = (unsigned char *)bm.Data() + bm.Pitch() * y;

                                    for (UINT x = 0; x < desc.Width; x++) {
                                        // memcpy(buf_des, buf_src, 3);
                                        buf_des[0] = buf_src[2];
                                        buf_des[1] = buf_src[1];
                                        buf_des[2] = buf_src[0];

                                        buf_src += 4;
                                        buf_des += 3;
                                    }
                                }

                                pSurface->UnlockRect();

                                bm.SaveInPNG(filename.c_str());
                                g_MatrixMap->m_DI.T((L"Screen shot has been saved into " + filename).c_str(), L"");
                            }
                            else {
                                // LockRect fail
                                // OutputDebugStringA("LockRect fail\n");
                            }
                        }
                        else {
                            // GetRenderTargetData fail
                            // char s[256];
                            // sprintf_s(s, sizeof(s), "GetRenderTargetData fail - 0x%08X, %u, %d\n", hr, hr, hr);
                            // OutputDebugStringA(s);
                        }
                        pSurface->Release();
                    }
                    else {
                        // CreateOffscreenPlainSurface fail
                        // OutputDebugStringA("CreateOffscreenPlainSurface fail\n");
                    }
                }
                else {
                    // GetDesc fail
                    // OutputDebugStringA("GetDesc fail\n");
                }

                if (pTargetSurface)
                    pTargetSurface->Release();
            }
            else {
                // GetRenderTarget fail
                // OutputDebugStringA("GetRenderTarget fail\n");
            }

            return;
        }

        CBitmap bm(g_CacheHeap);
        CBitmap bmout(g_CacheHeap);
        bmout.CreateRGB(g_ScreenX, g_ScreenY);

        HDC hdc = GetDC(g_Wnd);

        bm.WBM_Bitmap(CreateCompatibleBitmap(hdc, g_ScreenX, g_ScreenY));
        bm.WBM_BitmapDC(CreateCompatibleDC(hdc));
        if (SelectObject(bm.WBM_BitmapDC(), bm.WBM_Bitmap()) == 0) {
            ReleaseDC(g_Wnd, hdc);
            return;
        }
        BitBlt(bm.WBM_BitmapDC(), 0, 0, g_ScreenX, g_ScreenY, hdc, 0, 0, SRCCOPY);

        ReleaseDC(g_Wnd, hdc);

        bm.WBM_Save(true);

        bmout.Copy(CPoint(0, 0), bm.Size(), bm, CPoint(0, 0));
        bmout.SaveInPNG(filename.c_str());

        // HFree(data, g_CacheHeap);

        g_MatrixMap->m_DI.T(L"Screen shot has been saved", L"");
    }
}

void CNetGameForm::MouseMove(int x, int y) {
    DTRACE();

    CMatrixSideUnit *p_side = g_MatrixMap->GetPlayerSide();

    if (g_MatrixMap->IsMouseCam()) {
        g_MatrixMap->m_Camera.RotateByMouse(x - g_MatrixMap->m_Cursor.GetPosX(), y - g_MatrixMap->m_Cursor.GetPosY());

        CPoint p = g_MatrixMap->m_Cursor.GetPos();
        ClientToScreen(g_Wnd, (LPPOINT)&p);

        // SetCursorPos(p.x, p.y);

        if (p_side->GetArcadedObject()) {
            int dx = x - g_MatrixMap->m_Cursor.GetPosX();
            if (dx < 0)
                p_side->GetArcadedObject()->AsRobot()->RotateRobotLeft();
            if (dx > 0)
                p_side->GetArcadedObject()->AsRobot()->RotateRobotRight();
        }

        g_MatrixMap->m_Cursor.SetPos(x, y);

        return;
    }

    g_MatrixMap->m_Cursor.SetPos(x, y);
    p_side->OnMouseMove();

    if (CMultiSelection::m_GameSelection) {
        SCallback cbs;
        cbs.mp = CPoint(x, y);
        cbs.calls = 0;
        CMultiSelection::m_GameSelection->Update(g_MatrixMap->m_Cursor.GetPos(), TRACE_ROBOT | TRACE_BUILDING,
                                                 SideSelectionCallBack, (DWORD)&cbs);
    }

    // interface
    SETFLAG(g_IFaceList->m_IfListFlags, MINIMAP_ENABLE_DRAG);
}

void CNetGameForm::MouseKey(ButtonStatus status, int key, int x, int y) {
    DTRACE();

    if (status == B_WHEEL) {
        while (key > 0) {
            g_MatrixMap->m_Camera.ZoomInStep();
            --key;
        }
        while (key < 0) {
            g_MatrixMap->m_Camera.ZoomOutStep();
            ++key;
        }

        return;
    }

    DCP();

    if (status == B_UP && key == VK_MBUTTON) {
        g_MatrixMap->MouseCam(false);
        return;
    }
    if (status == B_DOWN && key == VK_MBUTTON) {
        g_MatrixMap->MouseCam(true);
        // SetCursorPos(g_ScreenX/2, g_ScreenY/2);

        return;
    }

    DCP();

    m_Action = 0;

    // bool fCtrl=(GetAsyncKeyState(VK_CONTROL) & 0x8000)==0x8000;

    /*
        if(fCtrl && down && key==VK_RBUTTON){
            D3DXVECTOR3 vpos,vdir;
            g_MatrixMap->CalcPickVector(CPoint(x,y), vdir);
            g_MatrixMap->UnitPickWorld(g_MatrixMap->GetFrustumCenter(),vdir,&m_LastWorldX,&m_LastWorldY);
            m_Action=1;
        }
    */
    DCP();

    if (status == B_UP && key == VK_LBUTTON) {
        DCP();
        CMatrixSideUnit *ps = g_MatrixMap->GetPlayerSide();
        if (CMultiSelection::m_GameSelection) {
            SCallback cbs;
            cbs.mp = CPoint(-1, -1);
            cbs.calls = 0;

            CMultiSelection::m_GameSelection->End();
            DCP();

            if (1 /*cbs.calls > 0*/) {
                DCP();
                if (ps->GetCurSelGroup()->GetFlyersCnt() > 1 || ps->GetCurSelGroup()->GetRobotsCnt() > 1 ||
                    (ps->GetCurSelGroup()->GetFlyersCnt() + ps->GetCurSelGroup()->GetRobotsCnt()) > 1) {
                    ps->GetCurSelGroup()->RemoveBuildings();
                    if ((GetAsyncKeyState(g_Config.m_KeyActions[KA_SHIFT]) & 0x8000) == 0x8000 && ps->GetCurGroup()) {
                        CMatrixGroupObject *go = ps->GetCurSelGroup()->m_FirstObject;
                        while (go) {
                            if (ps->GetCurGroup()->FindObject(go->GetObject())) {
                                CMatrixGroupObject *go_tmp = go->m_NextObject;
                                ps->RemoveObjectFromSelectedGroup(go->GetObject());
                                go = go_tmp;
                                continue;
                            }
                            go = go->m_NextObject;
                        }
                        ps->AddToCurrentGroup();
                    }
                    else {
                        ps->SetCurGroup(ps->CreateGroupFromCurrent());
                    }
                    if (ps->GetCurGroup() && ps->GetCurGroup()->GetRobotsCnt() && ps->GetCurGroup()->GetFlyersCnt()) {
                        ps->GetCurGroup()->SortFlyers();
                    }
                    ps->Select(GROUP, NULL);
                }
                else if (ps->GetCurSelGroup()->GetFlyersCnt() == 1 && !ps->GetCurSelGroup()->GetRobotsCnt()) {
                    DCP();
                    ps->GetCurSelGroup()->RemoveBuildings();
                    if ((GetAsyncKeyState(g_Config.m_KeyActions[KA_SHIFT]) & 0x8000) == 0x8000 && ps->GetCurGroup()) {
                        if (ps->GetCurGroup()->FindObject(ps->GetCurSelGroup()->m_FirstObject->GetObject())) {
                            ps->RemoveObjectFromSelectedGroup(ps->GetCurSelGroup()->m_FirstObject->GetObject());
                        }
                        else {
                            ps->AddToCurrentGroup();
                        }
                        if (ps->GetCurGroup() && ps->GetCurGroup()->GetRobotsCnt() &&
                            ps->GetCurGroup()->GetFlyersCnt()) {
                            ps->GetCurGroup()->SortFlyers();
                        }
                        ps->Select(GROUP, NULL);
                    }
                    else {
                        ps->SetCurGroup(ps->CreateGroupFromCurrent());
                        ps->Select(FLYER, NULL);
                    }
                }
                else if (ps->GetCurSelGroup()->GetRobotsCnt() == 1 && !ps->GetCurSelGroup()->GetFlyersCnt()) {
                    DCP();
                    ps->GetCurSelGroup()->RemoveBuildings();
                    if ((GetAsyncKeyState(g_Config.m_KeyActions[KA_SHIFT]) & 0x8000) == 0x8000 && ps->GetCurGroup()) {
                        if (ps->GetCurGroup()->FindObject(ps->GetCurSelGroup()->m_FirstObject->GetObject())) {
                            ps->RemoveObjectFromSelectedGroup(ps->GetCurSelGroup()->m_FirstObject->GetObject());
                        }
                        else {
                            ps->AddToCurrentGroup();
                        }

                        if (ps->GetCurGroup() && ps->GetCurGroup()->GetRobotsCnt() &&
                            ps->GetCurGroup()->GetFlyersCnt()) {
                            ps->GetCurGroup()->SortFlyers();
                        }
                        ps->Select(GROUP, NULL);
                    }
                    else {
                        ps->SetCurGroup(ps->CreateGroupFromCurrent());
                        ps->Select(ROBOT, NULL);
                    }
                }
                else if (ps->GetCurSelGroup()->GetBuildingsCnt() && !ps->GetCurSelGroup()->GetRobotsCnt() &&
                         !ps->GetCurSelGroup()->GetFlyersCnt()) {
                    DCP();
                    ps->Select(BUILDING, ps->GetCurSelGroup()->m_FirstObject->GetObject());
                    ps->GroupsUnselectSoft();
                    ps->GetCurSelGroup()->RemoveAll();
                    ps->SetCurGroup(NULL);
                    ps->Reselect();
                }
            }
        }
        CMultiSelection::m_GameSelection = NULL;
    }
    DCP();

    if (g_IFaceList->m_InFocus == INTERFACE) {
        DCP();
        if (status == B_UP && key == VK_LBUTTON) {
            g_IFaceList->OnMouseLBUp();
        }
        else if ((status == B_DOWN || status == B_DOUBLE) && key == VK_LBUTTON) {
            g_IFaceList->OnMouseLBDown();
        }
        if (status == B_DOWN && key == VK_RBUTTON) {
            g_IFaceList->OnMouseRBDown();
        }
    }
    else if (g_IFaceList->m_InFocus == UNKNOWN) {  // or something else
        DCP();
        if (status == B_DOWN && key == VK_RBUTTON) {
            DCP();
            g_MatrixMap->GetPlayerSide()->OnRButtonDown(CPoint(x, y));
        }
        else if (status == B_DOWN && key == VK_LBUTTON) {
            DCP();
            if (CMultiSelection::m_GameSelection == NULL && !g_MatrixMap->GetPlayerSide()->IsArcadeMode() &&
                !IS_PREORDERING_NOSELECT && !(g_MatrixMap->GetPlayerSide()->m_CurrentAction == BUILDING_TURRET)) {
                int dx = 0, dy = 0;
                if (IS_TRACE_STOP_OBJECT(g_MatrixMap->m_TraceStopObj) && IS_TRACE_UNIT(g_MatrixMap->m_TraceStopObj)) {
                    dx = 2;
                    dy = 2;
                }
                CMultiSelection::m_GameSelection = CMultiSelection::Begin(
                        CPoint(g_MatrixMap->m_Cursor.GetPos().x - dx, g_MatrixMap->m_Cursor.GetPos().y - dy));
                if (CMultiSelection::m_GameSelection) {
                    SCallback cbs;
                    cbs.mp = g_MatrixMap->m_Cursor.GetPos();
                    cbs.calls = 0;

                    CMultiSelection::m_GameSelection->Update(g_MatrixMap->m_Cursor.GetPos(),
                                                             TRACE_ROBOT | TRACE_BUILDING, SideSelectionCallBack,
                                                             (DWORD)&cbs);
                }
            }
            g_MatrixMap->GetPlayerSide()->OnLButtonDown(CPoint(x, y));
        }
        else if (status == B_UP && key == VK_RBUTTON) {
            DCP();
            g_MatrixMap->GetPlayerSide()->OnRButtonUp(CPoint(x, y));
        }
        else if (status == B_UP && key == VK_LBUTTON) {
            DCP();
            CMatrixSideUnit *ps = g_MatrixMap->GetPlayerSide();
            ps->OnLButtonUp(CPoint(x, y));
        }
        else if (status == B_DOUBLE && key == VK_LBUTTON) {
            DCP();
            g_MatrixMap->GetPlayerSide()->OnLButtonDouble(CPoint(x, y));
        }
        else if (status == B_DOUBLE && key == VK_RBUTTON) {
            DCP();
            g_MatrixMap->GetPlayerSide()->OnRButtonDouble(CPoint(x, y));
        }
    }
}

void ExitRequestHandler(void);
void ConfirmCancelHandler(void);
void ResetRequestHandler(void);
void SurrenderRequestHandler(void);

struct STextInfo {
    const wchar *t1;
    const wchar *t2;
    int time;
};

bool CNetGameForm::IsInputEqual(std::string str) {
    if (m_LastScans.size() < str.size()) {
        return false;
    }

    auto istr = str.rbegin();
    auto iscan = m_LastScans.rbegin();
    for (; istr != str.rend(); istr++, iscan++) {
        if (*istr != Scan2Char(iscan->scan)) {
            return false;
        }
    }

    return true;
}

void CNetGameForm::Keyboard(bool down, int scan) {
    DTRACE();

    bool fCtrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) == 0x8000;

    if (g_MatrixMap->m_Console.IsActive()) {
        // g_MatrixMap->m_Console.SetActive(true);
        g_MatrixMap->m_Console.Keyboard(scan, down);
        return;
    }

    if (down) {
        if (m_LastScans.size() == MAX_SCANS) {
            m_LastScans.pop_front();
        }
        m_LastScans.emplace_back(g_MatrixMap->GetTime(), scan);
    }

    if (scan == KEY_ENTER && down) {
        if (g_MatrixMap->m_DialogModeName && (g_MatrixMap->m_DialogModeHints.size() > 1 ||
                                              wcscmp(g_MatrixMap->m_DialogModeName, TEMPLATE_DIALOG_MENU) != 0)) {
            g_IFaceList->PressHintButton(HINT_OK);
            return;
        }
    }

    if (scan == KEY_E && down) {
        if (g_MatrixMap->m_DialogModeName && wcscmp(g_MatrixMap->m_DialogModeName, TEMPLATE_DIALOG_MENU) == 0) {
            if (g_MatrixMap->m_DialogModeHints.size() > 1) {}
            else {
                ExitRequestHandler();
                return;
            }
        }
    }
    if (scan == KEY_S && down) {
        if (g_MatrixMap->m_DialogModeName && wcscmp(g_MatrixMap->m_DialogModeName, TEMPLATE_DIALOG_MENU) == 0) {
            if (g_MatrixMap->m_DialogModeHints.size() > 1) {}
            else {
                SurrenderRequestHandler();
                return;
            }
        }
    }
    if (scan == KEY_R && down) {
        if (g_MatrixMap->m_DialogModeName && wcscmp(g_MatrixMap->m_DialogModeName, TEMPLATE_DIALOG_MENU) == 0) {
            if (g_MatrixMap->m_DialogModeHints.size() > 1) {}
            else {
                ResetRequestHandler();
                return;
            }
        }
    }

    if (scan == KEY_ESC && down) {
        if (FLAG(g_MatrixMap->m_Flags, MMFLAG_FULLAUTO)) {
            g_ExitState = 1;
            SETFLAG(g_Flags, GFLAG_EXITLOOP);

            return;
        }

        if (g_MatrixMap->m_DialogModeName && wcscmp(g_MatrixMap->m_DialogModeName, TEMPLATE_DIALOG_MENU) == 0) {
            if (g_MatrixMap->m_DialogModeHints.size() > 1) {
                ConfirmCancelHandler();
            }
            else {
                g_MatrixMap->LeaveDialogMode();
            }
            return;
        }

        if (FLAG(g_MatrixMap->m_Flags, MMFLAG_DIALOG_MODE))
            return;

        if (g_MatrixMap->GetPlayerSide()->GetArcadedObject()) {
            g_IFaceList->LiveRobot();
            return;
        }
        g_MatrixMap->EnterDialogMode(TEMPLATE_DIALOG_MENU);
        return;
    }

    g_MatrixMap->m_KeyDown = down;
    g_MatrixMap->m_KeyScan = scan;

    if (((GetAsyncKeyState(g_Config.m_KeyActions[KA_UNIT_FORWARD]) & 0x8000) == 0x8000) ||
        ((GetAsyncKeyState(g_Config.m_KeyActions[KA_UNIT_FORWARD_ALT]) & 0x8000) == 0x8000)) {
        g_MatrixMap->GetPlayerSide()->OnForward(true);
    }
    if (((GetAsyncKeyState(g_Config.m_KeyActions[KA_UNIT_BACKWARD]) & 0x8000) == 0x8000) ||
        ((GetAsyncKeyState(g_Config.m_KeyActions[KA_UNIT_BACKWARD_ALT]) & 0x8000) == 0x8000)) {
        g_MatrixMap->GetPlayerSide()->OnBackward(true);
    }

    if (down) {
        if (((GetAsyncKeyState(g_Config.m_KeyActions[KA_CAM_SETDEFAULT]) & 0x8000) == 0x8000)) {
            g_MatrixMap->m_Camera.ResetAngles();
            return;
        }

        CMatrixSideUnit *ps = g_MatrixMap->GetPlayerSide();
        if (scan == KEY_PAUSE) {
            g_MatrixMap->Pause(!g_MatrixMap->IsPaused());
            return;
        }

        if (ps->IsRobotMode()) {
            CMatrixRobotAI *robot = ps->GetArcadedObject()->AsRobot();

            if (((GetAsyncKeyState(g_Config.m_KeyActions[KA_UNIT_BOOM]) & 0x8000) == 0x8000)) {
                //"E" - ��������.
                robot->BigBoom();
                return;
            }
            if (((GetAsyncKeyState(g_Config.m_KeyActions[KA_UNIT_ENTER]) & 0x8000) == 0x8000) ||
                ((GetAsyncKeyState(g_Config.m_KeyActions[KA_UNIT_ENTER_ALT]) & 0x8000) == 0x8000)) {
                //"Esc", "������","Enter" - ����� � ����� �� ������.
                g_IFaceList->LiveRobot();
                return;
            }
        }
        else {
            CMatrixSideUnit *ps = g_MatrixMap->GetPlayerSide();
            if (!FLAG(g_IFaceList->m_IfListFlags, ORDERING_MODE) /*!IS_PREORDERING_NOSELECT*/) {
                // ���� �� �� � ������ �������

                if (ps->GetCurGroup() && (ps->m_CurrSel == ROBOT_SELECTED || ps->m_CurrSel == GROUP_SELECTED)) {
                    // �������������� ����� - ������� ������
                    if (((GetAsyncKeyState(g_Config.m_KeyActions[KA_UNIT_ENTER]) & 0x8000) == 0x8000) ||
                        ((GetAsyncKeyState(g_Config.m_KeyActions[KA_UNIT_ENTER_ALT]) & 0x8000) == 0x8000)) {
                        //"Esc", "������","Enter" - ����� � ����� �� ������.
                        CMatrixMapStatic *obj = ps->GetCurGroup()->GetObjectByN(ps->GetCurSelNum());
                        ps->GetCurSelGroup()->RemoveAll();
                        ps->CreateGroupFromCurrent(obj);
                        g_IFaceList->EnterRobot(false);
                    }
                    else if (((GetAsyncKeyState(g_Config.m_KeyActions[KA_AUTOORDER_ATTACK]) & 0x8000) == 0x8000)) {
                        // a"U"to attack - ��������� �����.
                        if (FLAG(g_IFaceList->m_IfListFlags, AUTO_FROBOT_ON)) {
                            RESETFLAG(g_IFaceList->m_IfListFlags, AUTO_FROBOT_ON | AUTO_CAPTURE_ON | AUTO_PROTECT_ON);
                            ps->PGOrderStop(ps->SelGroupToLogicGroup());
                        }
                        else {
                            RESETFLAG(g_IFaceList->m_IfListFlags, AUTO_FROBOT_ON | AUTO_CAPTURE_ON | AUTO_PROTECT_ON);
                            SETFLAG(g_IFaceList->m_IfListFlags, AUTO_FROBOT_ON);
                            ps->PGOrderAutoAttack(ps->SelGroupToLogicGroup());
                        }
                    }
                    else if (((GetAsyncKeyState(g_Config.m_KeyActions[KA_AUTOORDER_CAPTURE]) & 0x8000) == 0x8000)) {
                        //"C"apture - ��������� �������.
                        if (FLAG(g_IFaceList->m_IfListFlags, AUTO_CAPTURE_ON)) {
                            RESETFLAG(g_IFaceList->m_IfListFlags, AUTO_FROBOT_ON | AUTO_CAPTURE_ON | AUTO_PROTECT_ON);
                            ps->PGOrderStop(ps->SelGroupToLogicGroup());
                        }
                        else {
                            RESETFLAG(g_IFaceList->m_IfListFlags, AUTO_FROBOT_ON | AUTO_CAPTURE_ON | AUTO_PROTECT_ON);
                            SETFLAG(g_IFaceList->m_IfListFlags, AUTO_CAPTURE_ON);
                            ps->PGOrderAutoCapture(ps->SelGroupToLogicGroup());
                        }
                    }
                    else if (((GetAsyncKeyState(g_Config.m_KeyActions[KA_AUTOORDER_DEFEND]) & 0x8000) == 0x8000)) {
                        //"D"efender - ��������� �������� Protect
                        if (FLAG(g_IFaceList->m_IfListFlags, AUTO_PROTECT_ON)) {
                            RESETFLAG(g_IFaceList->m_IfListFlags, AUTO_FROBOT_ON | AUTO_CAPTURE_ON | AUTO_PROTECT_ON);
                            ps->PGOrderStop(ps->SelGroupToLogicGroup());
                        }
                        else {
                            RESETFLAG(g_IFaceList->m_IfListFlags, AUTO_FROBOT_ON | AUTO_CAPTURE_ON | AUTO_PROTECT_ON);
                            SETFLAG(g_IFaceList->m_IfListFlags, AUTO_PROTECT_ON);
                            ps->PGOrderAutoDefence(ps->SelGroupToLogicGroup());
                        }
                    }
                    else if (((GetAsyncKeyState(g_Config.m_KeyActions[KA_ORDER_MOVE]) & 0x8000) == 0x8000)) {
                        //"M"ove - ���������
                        SETFLAG(g_IFaceList->m_IfListFlags, PREORDER_MOVE);
                        SETFLAG(g_IFaceList->m_IfListFlags, ORDERING_MODE);
                    }
                    else if (((GetAsyncKeyState(g_Config.m_KeyActions[KA_ORDER_STOP]) & 0x8000) == 0x8000)) {
                        //"S"top - ������
                        ps->PGOrderStop(ps->SelGroupToLogicGroup());
                        ps->SelectedGroupBreakOrders();
                    }
                    else if (((GetAsyncKeyState(g_Config.m_KeyActions[KA_ORDER_CAPTURE]) & 0x8000) == 0x8000)) {
                        // Ta"K"e - ���������
                        SETFLAG(g_IFaceList->m_IfListFlags, PREORDER_CAPTURE);
                        SETFLAG(g_IFaceList->m_IfListFlags, ORDERING_MODE);
                    }
                    else if (((GetAsyncKeyState(g_Config.m_KeyActions[KA_ORDER_PATROL]) & 0x8000) == 0x8000)) {
                        //"P"atrol - �������������
                        SETFLAG(g_IFaceList->m_IfListFlags, PREORDER_PATROL);
                        SETFLAG(g_IFaceList->m_IfListFlags, ORDERING_MODE);
                    }
                    else if (ps->GetCurGroup()->GetBombersCnt() &&
                             ((GetAsyncKeyState(g_Config.m_KeyActions[KA_ORDER_EXPLODE]) & 0x8000) == 0x8000)) {
                        //"E"xplode - ��������
                        SETFLAG(g_IFaceList->m_IfListFlags, PREORDER_BOMB);
                        SETFLAG(g_IFaceList->m_IfListFlags, ORDERING_MODE);
                    }
                    else if (ps->GetCurGroup()->GetRepairsCnt() &&
                             ((GetAsyncKeyState(g_Config.m_KeyActions[KA_ORDER_REPAIR]) & 0x8000) == 0x8000)) {
                        //"R"epair - ������
                        SETFLAG(g_IFaceList->m_IfListFlags, PREORDER_REPAIR);
                        SETFLAG(g_IFaceList->m_IfListFlags, ORDERING_MODE);
                    }
                    else if (((GetAsyncKeyState(g_Config.m_KeyActions[KA_ORDER_ATTACK]) & 0x8000) == 0x8000)) {
                        //"A"ttack
                        SETFLAG(g_IFaceList->m_IfListFlags, PREORDER_FIRE);
                        SETFLAG(g_IFaceList->m_IfListFlags, ORDERING_MODE);
                    }
                }
                else if (ps->m_CurrSel == BUILDING_SELECTED || ps->m_CurrSel == BASE_SELECTED) {
                    // �������������� ����� - ������� ����\�����
                    CMatrixBuilding *bld = (CMatrixBuilding *)ps->m_ActiveObject;

                    if (bld->IsBase() && !ps->m_ConstructPanel->IsActive() &&
                        ((GetAsyncKeyState(g_Config.m_KeyActions[KA_BUILD_ROBOT]) & 0x8000) == 0x8000)) {
                        //"B"ase - ���� � ��������� ������
                        g_IFaceList->m_RCountControl->Reset();
                        g_IFaceList->m_RCountControl->CheckUp();
                        ps->m_ConstructPanel->ActivateAndSelect();
                    }
                    else if (((GetAsyncKeyState(g_Config.m_KeyActions[KA_BUILD_TURRET]) & 0x8000) == 0x8000)) {
                        //"T"urrel - ������� � ���� ������ �������
                        CPoint pl[MAX_PLACES];
                        bool cant_build_tu = false;

                        if ((ps->IsEnoughResources(g_Config.m_CannonsProps[0].m_Resources) ||
                             ps->IsEnoughResources(g_Config.m_CannonsProps[1].m_Resources) ||
                             ps->IsEnoughResources(g_Config.m_CannonsProps[2].m_Resources) ||
                             ps->IsEnoughResources(g_Config.m_CannonsProps[3].m_Resources)) &&
                            (bld->GetPlacesForTurrets(pl) > 0) && (!bld->m_BS.IsMaxItems())) {
                            ps->m_ConstructPanel->ResetGroupNClose();
                            SETFLAG(g_IFaceList->m_IfListFlags, ORDERING_MODE);
                            SETFLAG(g_IFaceList->m_IfListFlags, PREORDER_BUILD_TURRET);

                            CMatrixMapStatic *ms = CMatrixMapStatic::GetFirstLogic();
                            for (; ms; ms = ms->GetNextLogic()) {
                                if (ms == ps->m_ActiveObject && ms->IsLiveBuilding() &&
                                    ms->AsBuilding()->m_Side == PLAYER_SIDE) {
                                    ms->AsBuilding()->CreatePlacesShow();
                                    break;
                                }
                            }
                        }
                    }
                    else if (((GetAsyncKeyState(g_Config.m_KeyActions[KA_BUILD_HELP]) & 0x8000) == 0x8000)) {
                        //"�"elp - ����� ������������
                        bld->Maintenance();
                    }
                    else if (scan > 1 && scan <= 11 && ps->m_ConstructPanel->IsActive() &&
                             ps->m_ConstructPanel->m_FocusedElement) {
                        int key = 0;
                        if (scan != 11)
                            key = scan - 1;

                        ERobotUnitType type = MRT_EMPTY;
                        ERobotUnitKind kind = RUK_UNKNOWN;
                        int pilon = -1;

                        // RUK_WEAPON_MACHINEGUN      = 1,  1
                        // RUK_WEAPON_CANNON          = 2,  2
                        // RUK_WEAPON_MISSILE         = 3,  3
                        // RUK_WEAPON_FLAMETHROWER    = 4,  4
                        // RUK_WEAPON_MORTAR          = 5,
                        // RUK_WEAPON_LASER           = 6,  5
                        // RUK_WEAPON_BOMB            = 7,
                        // RUK_WEAPON_PLASMA          = 8,  6
                        // RUK_WEAPON_ELECTRIC        = 9,  7
                        // RUK_WEAPON_REPAIR          = 10, 8

                        if (ps->m_ConstructPanel->m_FocusedElement->m_strName == IF_BASE_PILON1) {
                            pilon = 0;
                            type = MRT_WEAPON;
                        }
                        else if (ps->m_ConstructPanel->m_FocusedElement->m_strName == IF_BASE_PILON2) {
                            pilon = 1;
                            type = MRT_WEAPON;
                        }
                        else if (ps->m_ConstructPanel->m_FocusedElement->m_strName == IF_BASE_PILON3) {
                            pilon = 2;
                            type = MRT_WEAPON;
                        }
                        else if (ps->m_ConstructPanel->m_FocusedElement->m_strName == IF_BASE_PILON4) {
                            pilon = 3;
                            type = MRT_WEAPON;
                        }
                        else if (key <= 2 && ps->m_ConstructPanel->m_FocusedElement->m_strName == IF_BASE_PILON5) {
                            pilon = 4;
                            type = MRT_WEAPON;

                            kind = ERobotUnitKind(key);

                            if (key == 1) {
                                kind = RUK_WEAPON_MORTAR;
                            }
                            else if (key == 2) {
                                kind = RUK_WEAPON_BOMB;
                            }
                        }
                        else if (key <= 4 && ps->m_ConstructPanel->m_FocusedElement->m_strName == IF_BASE_PILON_HEAD) {
                            pilon = 0;
                            type = MRT_HEAD;

                            kind = ERobotUnitKind(key);
                        }
                        else if (key && key <= 6 &&
                                 ps->m_ConstructPanel->m_FocusedElement->m_strName == IF_BASE_PILON_HULL) {
                            pilon = 0;
                            type = MRT_ARMOR;

                            if (key == 1)
                                kind = ERobotUnitKind(6);
                            else
                                kind = ERobotUnitKind(key - 1);
                        }
                        else if (key && key <= 5 &&
                                 ps->m_ConstructPanel->m_FocusedElement->m_strName == IF_BASE_PILON_CHASSIS) {
                            pilon = 0;
                            type = MRT_CHASSIS;
                            kind = ERobotUnitKind(key);
                        }

                        if (key <= 8 && type == MRT_WEAPON && pilon < 4) {
                            if (key == 0) {
                                kind = ERobotUnitKind(0);
                            }
                            else if (key == 5) {
                                kind = ERobotUnitKind(key + 1);
                            }
                            else if (key > 5) {
                                kind = ERobotUnitKind(key + 2);
                            }
                            else {
                                kind = ERobotUnitKind(key);
                            }
                        }
                        if (int(type))
                            ps->m_Constructor->SuperDjeans(type, kind, pilon);
                    }
                }
            }
            else {
                // ���� �� � ������ ������� ��� ��������� �����

                if (FLAG(g_IFaceList->m_IfListFlags, ORDERING_MODE) &&
                    FLAG(g_IFaceList->m_IfListFlags, PREORDER_BUILD_TURRET) && ps->m_CurrentAction != BUILDING_TURRET) {
                    // player_side->IsEnoughResources(g_Config.m_CannonsProps[1].m_Resources)
                    // ���� ������ �������:
                    if (((GetAsyncKeyState(g_Config.m_KeyActions[KA_TURRET_CANNON]) & 0x8000) == 0x8000)) {
                        //"C"annon - ������
                        g_IFaceList->BeginBuildTurret(1);
                    }
                    else if (((GetAsyncKeyState(g_Config.m_KeyActions[KA_TURRET_GUN]) & 0x8000) == 0x8000)) {
                        //"G"un - �����
                        g_IFaceList->BeginBuildTurret(2);
                    }
                    else if (((GetAsyncKeyState(g_Config.m_KeyActions[KA_TURRET_LASER]) & 0x8000) == 0x8000)) {
                        //"L"azer - �����
                        g_IFaceList->BeginBuildTurret(3);
                    }
                    else if (((GetAsyncKeyState(g_Config.m_KeyActions[KA_TURRET_ROCKET]) & 0x8000) == 0x8000)) {
                        //"R"ocket - ���������
                        g_IFaceList->BeginBuildTurret(4);
                    }
                }

                if (((GetAsyncKeyState(g_Config.m_KeyActions[KA_ORDER_CANCEL]) & 0x8000) == 0x8000)) {
                    // ������
                    if (ps->m_CurrentAction == BUILDING_TURRET) {
                        ps->m_CannonForBuild.Delete();
                        ps->m_CurrentAction = NOTHING_SPECIAL;
                    }
                    g_IFaceList->ResetOrderingMode();
                }
            }
            // ����� ��� ��������������� ������
            if (((GetAsyncKeyState(g_Config.m_KeyActions[KA_MINIMAP_ZOOMIN]) & 0x8000) == 0x8000)) {
                // ���������� �����
                g_MatrixMap->m_Minimap.ButtonZoomIn(NULL);
            }
            else if (((GetAsyncKeyState(g_Config.m_KeyActions[KA_MINIMAP_ZOOMOUT]) & 0x8000) == 0x8000)) {
                // �������� �����
                g_MatrixMap->m_Minimap.ButtonZoomOut(NULL);
            }
            else if (((GetAsyncKeyState(g_Config.m_KeyActions[KA_ORDER_ROBOT_SWITCH1]) & 0x8000) == 0x8000)) {
                //","
                CMatrixMapStatic *obj = CMatrixMapStatic::GetFirstLogic();
                if (ps->GetCurGroup() && ps->GetCurGroup()->GetObjectsCnt() == 1 &&
                    ps->GetCurGroup()->m_FirstObject->GetObject()->IsLiveRobot()) {
                    obj = ps->GetCurGroup()->m_FirstObject->GetObject()->GetPrevLogic();
                }
                int cnt = 0;
                while (1) {
                    if (obj) {
                        if (obj->IsLiveRobot() && obj->GetSide() == PLAYER_SIDE) {
                            ps->GetCurSelGroup()->RemoveAll();
                            ps->CreateGroupFromCurrent(obj);
                            ps->Select(ROBOT, obj);
                            g_MatrixMap->m_Camera.SetXYStrategy(
                                    D3DXVECTOR2(obj->GetGeoCenter().x, obj->GetGeoCenter().y));
                            return;
                        }
                        obj = obj->GetPrevLogic();
                    }
                    else {
                        if (cnt > 0) {
                            return;
                        }
                        cnt++;
                        obj = CMatrixMapStatic::GetLastLogic();
                    }
                }
            }
            else if (((GetAsyncKeyState(g_Config.m_KeyActions[KA_ORDER_ROBOT_SWITCH2]) & 0x8000) == 0x8000)) {
                //"."
                CMatrixMapStatic *obj = CMatrixMapStatic::GetFirstLogic();
                if (ps->GetCurGroup() && ps->GetCurGroup()->GetObjectsCnt() == 1 &&
                    ps->GetCurGroup()->m_FirstObject->GetObject()->IsLiveRobot()) {
                    obj = ps->GetCurGroup()->m_FirstObject->GetObject()->GetNextLogic();
                }
                int cnt = 0;
                while (1) {
                    if (obj) {
                        if (obj->IsLiveRobot() && obj->GetSide() == PLAYER_SIDE) {
                            ps->GetCurSelGroup()->RemoveAll();
                            ps->CreateGroupFromCurrent(obj);
                            ps->Select(ROBOT, obj);
                            g_MatrixMap->m_Camera.SetXYStrategy(
                                    D3DXVECTOR2(obj->GetGeoCenter().x, obj->GetGeoCenter().y));
                            return;
                        }
                        obj = obj->GetNextLogic();
                    }
                    else {
                        if (cnt > 0) {
                            return;
                        }
                        cnt++;
                        obj = CMatrixMapStatic::GetFirstLogic();
                    }
                }
            }
        }

        if (scan > 1 && scan < 11 && !ps->IsArcadeMode() && !ps->m_ConstructPanel->IsActive()) {
            if (CMultiSelection::m_GameSelection) {
                CMultiSelection::m_GameSelection->End(false);
            }

            if (ps->m_CurrentAction == BUILDING_TURRET) {
                ps->m_CannonForBuild.Delete();
                ps->m_CurrentAction = NOTHING_SPECIAL;
            }

            g_IFaceList->ResetOrderingMode();

            if (fCtrl) {
                CMatrixMapStatic *o = CMatrixMapStatic::GetFirstLogic();
                while (o) {
                    if (o->IsRobot() && ((CMatrixRobotAI *)o)->GetCtrlGroup() == scan) {
                        ((CMatrixRobotAI *)o)->SetCtrlGroup(0);
                    }
                    else if (o->GetObjectType() == OBJECT_TYPE_FLYER && ((CMatrixFlyer *)o)->GetCtrlGroup() == scan) {
                        ((CMatrixFlyer *)o)->SetCtrlGroup(0);
                    }
                    o = o->GetNextLogic();
                }

                if (ps->GetCurGroup()) {
                    CMatrixGroupObject *go = ps->GetCurGroup()->m_FirstObject;
                    while (go) {
                        if (go->GetObject()->GetObjectType() == OBJECT_TYPE_ROBOTAI) {
                            ((CMatrixRobotAI *)go->GetObject())->SetCtrlGroup(scan);
                        }
                        else if (go->GetObject()->GetObjectType() == OBJECT_TYPE_FLYER) {
                            ((CMatrixFlyer *)go->GetObject())->SetCtrlGroup(scan);
                        }
                        go = go->m_NextObject;
                    }
                }
            }
            else {
                CMatrixMapStatic *o = CMatrixMapStatic::GetFirstLogic();

                auto iscan = m_LastScans.rbegin();
                bool prev_unselected = false;
                if (!ps->GetCurGroup()) {
                    prev_unselected = true;
                }
                else if (ps->GetCurGroup() && iscan->scan == scan && (iscan + 1)->scan == scan &&
                         (iscan->time - (iscan + 1)->time) < DOUBLESCAN_TIME_DELTA) {
                    CMatrixMapStatic *object = NULL;
                    if (ps->GetCurGroup()->m_FirstObject)
                        object = ps->GetCurGroup()->m_FirstObject->GetObject();

                    if (object && object->GetObjectType() == OBJECT_TYPE_ROBOTAI) {
                        if (((CMatrixRobotAI *)object)->GetCtrlGroup() == scan) {
                            // set camera to group position. out

                            g_MatrixMap->m_Camera.SetXYStrategy(
                                    D3DXVECTOR2(object->GetGeoCenter().x, object->GetGeoCenter().y + 100.0f));
                            return;
                        }
                    }
                    else if (object && object->GetObjectType() == OBJECT_TYPE_FLYER) {
                        if (((CMatrixFlyer *)object)->GetCtrlGroup() == scan) {
                            // set camera to group position. out
                            g_MatrixMap->m_Camera.SetXYStrategy(
                                    D3DXVECTOR2(object->GetGeoCenter().x, object->GetGeoCenter().y + 100.0f));
                            return;
                        }
                    }
                }

                while (o) {
                    if (o->GetSide() == PLAYER_SIDE) {
                        if (o->IsLiveRobot() && o->AsRobot()->GetCtrlGroup() == scan) {
                            if (!prev_unselected) {
                                prev_unselected = true;
                                ps->SelectedGroupUnselect();
                                ps->GetCurSelGroup()->RemoveAll();
                            }
                            ps->GetCurSelGroup()->AddObject(o, -4);
                        }
                    }
                    o = o->GetNextLogic();
                }
                ///
                if (ps->GetCurSelGroup()->GetFlyersCnt() > 1 || ps->GetCurSelGroup()->GetRobotsCnt() > 1 ||
                    (ps->GetCurSelGroup()->GetFlyersCnt() + ps->GetCurSelGroup()->GetRobotsCnt()) > 1) {
                    ps->CreateGroupFromCurrent();
                    if (ps->GetCurGroup() && ps->GetCurGroup()->GetRobotsCnt() && ps->GetCurGroup()->GetFlyersCnt()) {
                        ps->GetCurGroup()->SortFlyers();
                    }
                    ps->Select(GROUP, NULL);
                }
                else if (ps->GetCurSelGroup()->GetFlyersCnt() == 1 && !ps->GetCurSelGroup()->GetRobotsCnt()) {
                    ps->CreateGroupFromCurrent();
                    ps->Select(FLYER, NULL);
                }
                else if (ps->GetCurSelGroup()->GetRobotsCnt() == 1 && !ps->GetCurSelGroup()->GetFlyersCnt()) {
                    ps->CreateGroupFromCurrent();
                    ps->Select(ROBOT, NULL);
                }
            }
        }
        // BUTTON UNPRESSED
        if (!down) {
            if (scan == KEY_LSHIFT) {}
        }
    }
}

void CNetGameForm::SystemEvent(ESysEvent se) {
    DTRACE();
    if (se == SYSEV_DEACTIVATING) {
        if (FLAG(g_MatrixMap->m_Flags, MMFLAG_VIDEO_RESOURCES_READY)) {
            g_MatrixMap->ReleasePoolDefaultResources();
        }
    }

    if (se == SYSEV_ACTIVATED) {
        if (FLAG(g_Flags, GFLAG_FULLSCREEN)) {
            RECT r;
            GetWindowRect(g_Wnd, &r);
            ClipCursor(&r);
        }
    }
}