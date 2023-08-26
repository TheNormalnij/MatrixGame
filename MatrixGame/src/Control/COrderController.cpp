// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2023, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "COrderController.h"

#include "Interface/CInterface.h"
#include "MatrixObjectCannon.hpp"
#include "MatrixMapUtils.h"

// Этот код полный кусок говна
// Функционал нужно пораскидывать по соответствующим классам
// Является уменьшенной частью MatrixSide.cpp

COrderController::COrderController(IOrderProcessor *processor, CMatrixSideUnit *playerSide) {
    m_pOrderProcessor = processor;
    m_pPlayerSide = playerSide;
}

void COrderController::OnMouseMove() {
    // Do nothing
}

void COrderController::OnLButtonDown(const CPoint &) {
    //DTRACE();
    if (IsArcadeMode())
        return;

    CMatrixMapStatic *pObject = m_pPlayerSide->MouseToLand();

    if (pObject == TRACE_STOP_NONE)
        return;

    auto m_CannonForBuild = &m_pPlayerSide->m_CannonForBuild;

    if (m_pPlayerSide->m_CurrentAction == EPlayerActions::BUILDING_TURRET && m_CannonForBuild->m_Cannon && m_CannonForBuild->m_CanBuildFlag /* && (m_CannonForBuild.m_ParentBuilding->m_TurretsHave < m_CannonForBuild.m_ParentBuilding->m_TurretsMax)*/) {
        if (g_MatrixMap->IsPaused())
            return;

        m_pOrderProcessor->BuildTurret((CMatrixBuilding *)m_pPlayerSide->m_ActiveObject,
                                       m_CannonForBuild->m_Cannon->m_Pos.x,
                                       m_CannonForBuild->m_Cannon->m_Pos.y, m_CannonForBuild->m_Cannon->GetMustBeAngle(),
                                       m_CannonForBuild->m_Cannon->m_Place, m_CannonForBuild->m_Cannon->m_Num);


        m_pPlayerSide->m_CurrentAction = NOTHING_SPECIAL;
        m_CannonForBuild->Delete();
        CSound::Play(S_TURRET_BUILD_START, SL_ALL);
        g_IFaceList->ResetBuildCaMode();
        return;
    }

    if (m_pPlayerSide->m_CurrentAction == BUILDING_TURRET && m_pPlayerSide->m_CannonForBuild.m_Cannon) {
        return;
    }
    int mx = Float2Int(g_MatrixMap->m_TraceStopPos.x / GLOBAL_SCALE_MOVE);
    int my = Float2Int(g_MatrixMap->m_TraceStopPos.y / GLOBAL_SCALE_MOVE);
    D3DXVECTOR3 tpos = g_MatrixMap->m_TraceStopPos;

    if (g_IFaceList->m_InFocus == INTERFACE && g_IFaceList->m_FocusedInterface->m_strName == IF_MINI_MAP) {
        D3DXVECTOR2 tgt;
        if (g_MatrixMap->m_Minimap.CalcMinimap2World(tgt)) {
            pObject = TRACE_STOP_LANDSCAPE;
            mx = Float2Int(tgt.x / GLOBAL_SCALE_MOVE);
            my = Float2Int(tgt.y / GLOBAL_SCALE_MOVE);
            tpos = D3DXVECTOR3(tgt.x, tgt.y, tpos.z);
        }
    }

    if (IS_PREORDERING) {
        if (FLAG(g_IFaceList->m_IfListFlags, PREORDER_MOVE)) {
            // Move
            RESETFLAG(g_IFaceList->m_IfListFlags, PREORDER_MOVE | ORDERING_MODE);

            m_pPlayerSide->PGOrderMoveTo(m_pPlayerSide->SelGroupToLogicGroup(),
                          CPoint(mx - ROBOT_MOVECELLS_PER_SIZE / 2, my - ROBOT_MOVECELLS_PER_SIZE / 2));

            CMatrixGroup *group = m_pPlayerSide->GetCurGroup();
            CMatrixGroupObject *objs = group->m_FirstObject;
            while (objs) {
                if (objs->GetObject() && objs->GetObject()->GetObjectType() == OBJECT_TYPE_FLYER) {
                    int param = (group->GetObjectsCnt() - group->GetRobotsCnt()) - 1;
                    if (param > 4)
                        param = 4;
                    float x = (float)g_MatrixMap->RndFloat(g_MatrixMap->m_TraceStopPos.x - param * GLOBAL_SCALE_MOVE,
                                                           g_MatrixMap->m_TraceStopPos.x + param * GLOBAL_SCALE_MOVE);
                    float y = (float)g_MatrixMap->RndFloat(g_MatrixMap->m_TraceStopPos.y - param * GLOBAL_SCALE_MOVE,
                                                           g_MatrixMap->m_TraceStopPos.y + param * GLOBAL_SCALE_MOVE);
                    ((CMatrixFlyer *)objs->GetObject())->SetTarget(D3DXVECTOR2(x, y));
                }
                objs = objs->m_NextObject;
            }
        }
        else if (FLAG(g_IFaceList->m_IfListFlags, PREORDER_FIRE)) {
            // Fire
            if (IS_TRACE_STOP_OBJECT(pObject) && (pObject->IsLive() || pObject->IsSpecial())) {
                RESETFLAG(g_IFaceList->m_IfListFlags, PREORDER_FIRE | ORDERING_MODE);

                m_pPlayerSide->PGOrderAttack(m_pPlayerSide->SelGroupToLogicGroup(), GetMapPos(pObject),
                                             pObject);
            }
            else {
                RESETFLAG(g_IFaceList->m_IfListFlags, PREORDER_FIRE | ORDERING_MODE);

                m_pPlayerSide->PGOrderAttack(
                        m_pPlayerSide->SelGroupToLogicGroup(),
                              CPoint(mx - ROBOT_MOVECELLS_PER_SIZE / 2, my - ROBOT_MOVECELLS_PER_SIZE / 2), NULL);
            }
        }
        else if (FLAG(g_IFaceList->m_IfListFlags, PREORDER_CAPTURE)) {
            // Capture
            if (IS_TRACE_STOP_OBJECT(pObject) && pObject->IsLiveBuilding() && pObject->GetSide() != PLAYER_SIDE) {
                RESETFLAG(g_IFaceList->m_IfListFlags, PREORDER_CAPTURE | ORDERING_MODE);

                m_pPlayerSide->PGOrderCapture(m_pPlayerSide->SelGroupToLogicGroup(), (CMatrixBuilding *)pObject);
            }
        }
        else if (FLAG(g_IFaceList->m_IfListFlags, PREORDER_PATROL)) {
            // Patrol
            RESETFLAG(g_IFaceList->m_IfListFlags, PREORDER_PATROL | ORDERING_MODE);
            m_pPlayerSide->PGOrderPatrol(m_pPlayerSide->SelGroupToLogicGroup(),
                          CPoint(mx - ROBOT_MOVECELLS_PER_SIZE / 2, my - ROBOT_MOVECELLS_PER_SIZE / 2));
        }
        else if (FLAG(g_IFaceList->m_IfListFlags, PREORDER_BOMB)) {
            // Nuclear BOMB!!! spasaisya kto mozhet!!! dab shas rvanet bombu!!!!
            if (IS_TRACE_STOP_OBJECT(pObject) && (pObject->IsLive() || pObject->IsSpecial())) {
                RESETFLAG(g_IFaceList->m_IfListFlags, PREORDER_BOMB | ORDERING_MODE);

                m_pPlayerSide->PGOrderBomb(m_pPlayerSide->SelGroupToLogicGroup(), GetMapPos(pObject), pObject);
            }
            else {
                RESETFLAG(g_IFaceList->m_IfListFlags, PREORDER_BOMB | ORDERING_MODE);

                m_pPlayerSide->PGOrderBomb(m_pPlayerSide->SelGroupToLogicGroup(),
                            CPoint(mx - ROBOT_MOVECELLS_PER_SIZE / 2, my - ROBOT_MOVECELLS_PER_SIZE / 2), NULL);
            }
        }
        else if (FLAG(g_IFaceList->m_IfListFlags, PREORDER_REPAIR)) {
            // Repair our robots please
            if (IS_TRACE_STOP_OBJECT(pObject) && pObject->IsLive() && pObject->GetSide() == PLAYER_SIDE) {
                RESETFLAG(g_IFaceList->m_IfListFlags, PREORDER_REPAIR | ORDERING_MODE);

                m_pPlayerSide->PGOrderRepair(m_pPlayerSide->SelGroupToLogicGroup(), (CMatrixBuilding *)pObject);
            }
        }
    }
}

void COrderController::OnLButtonDouble(const CPoint &mouse) {
    DTRACE();
    if (IsArcadeMode())
        return;

    CMatrixMapStatic *pObject = m_pPlayerSide->MouseToLand();

    if (pObject == TRACE_STOP_NONE ||
        !(IS_TRACE_STOP_OBJECT(pObject) && pObject->IsLiveRobot() && pObject->GetSide() == PLAYER_SIDE))
        return;

    if (IS_TRACE_STOP_OBJECT(pObject) && pObject->IsLiveRobot() && pObject->GetSide() == PLAYER_SIDE) {
        D3DXVECTOR3 o_pos = pObject->GetGeoCenter();
        CMatrixMapStatic *st = CMatrixMapStatic::GetFirstLogic();

        if (m_pPlayerSide->GetCurGroup()) {
            m_pPlayerSide->SelectedGroupUnselect();
            m_pPlayerSide->GetCurSelGroup()->RemoveAll();
        }

        while (st) {
            if (st->GetSide() == PLAYER_SIDE && st->IsLiveRobot()) {
                auto tmp = o_pos - st->GetGeoCenter();
                if (D3DXVec3LengthSq(&tmp) <= FRIENDLY_SEARCH_RADIUS * FRIENDLY_SEARCH_RADIUS) {
                    m_pPlayerSide->GetCurSelGroup()->AddObject(st, -4);
                }
            }
            st = st->GetNextLogic();
        }
    }
    m_pPlayerSide->CreateGroupFromCurrent();
    if (m_pPlayerSide->GetCurGroup() && m_pPlayerSide->GetCurGroup()->GetObjectsCnt() == 1) {
        m_pPlayerSide->Select(ESelType::ROBOT, NULL);
    }
    else if (m_pPlayerSide->GetCurGroup() && m_pPlayerSide->GetCurGroup()->GetObjectsCnt() > 1) {
        m_pPlayerSide->Select(ESelType::GROUP, NULL);
    }
}

void COrderController::OnLButtonUp(const CPoint &) {
    // Do nothing
}

void COrderController::OnRButtonDown(const CPoint &) {
    DTRACE();
    if (IsArcadeMode())
        return;
    DCP();
    if (IS_PREORDERING && m_pPlayerSide->m_CurrentAction == BUILDING_TURRET) {
        DCP();
        g_IFaceList->ResetOrderingMode();
        m_pPlayerSide->m_CannonForBuild.Delete();
        m_pPlayerSide->m_CurrentAction = NOTHING_SPECIAL;
        return;
    }
    DCP();

    CMatrixMapStatic *pObject = m_pPlayerSide->MouseToLand();
    DCP();

    int mx = Float2Int(g_MatrixMap->m_TraceStopPos.x / GLOBAL_SCALE_MOVE);
    int my = Float2Int(g_MatrixMap->m_TraceStopPos.y / GLOBAL_SCALE_MOVE);
    D3DXVECTOR3 tpos = g_MatrixMap->m_TraceStopPos;

    DCP();
    if (!IS_PREORDERING && m_pPlayerSide->GetCurGroup() && g_IFaceList->m_InFocus == INTERFACE &&
        g_IFaceList->m_FocusedInterface->m_strName == IF_MINI_MAP) {
        D3DXVECTOR2 tgt;
        if (g_MatrixMap->m_Minimap.CalcMinimap2World(tgt)) {
            pObject = TRACE_STOP_LANDSCAPE;
            mx = Float2Int(tgt.x / GLOBAL_SCALE_MOVE);
            my = Float2Int(tgt.y / GLOBAL_SCALE_MOVE);
            tpos = D3DXVECTOR3(tgt.x, tgt.y, tpos.z);
            g_MatrixMap->m_Minimap.AddEvent(tpos.x, tpos.y, 0xffff0000, 0xffff0000);
        }
    }

    if (pObject == TRACE_STOP_NONE)
        return;

    if (!IS_PREORDERING && (m_pPlayerSide->m_CurrSel == GROUP_SELECTED || m_pPlayerSide->m_CurrSel == ROBOT_SELECTED ||
                            m_pPlayerSide->m_CurrSel == FLYER_SELECTED)) {
        if (IS_TRACE_STOP_OBJECT(pObject) && pObject->IsLiveBuilding() && pObject->GetSide() != m_pPlayerSide->GetId()) {
            // Capture
            m_pPlayerSide->PGOrderCapture(m_pPlayerSide->SelGroupToLogicGroup(), (CMatrixBuilding *)pObject);
        }
        else if (IS_TRACE_STOP_OBJECT(pObject) &&
                 ((IsLiveUnit(pObject) && pObject->GetSide() != m_pPlayerSide->GetId()) ||
                  pObject->IsSpecial())) {
            // Attack
            m_pPlayerSide->PGOrderAttack(m_pPlayerSide->SelGroupToLogicGroup(), GetMapPos(pObject), pObject);
        }
        else if (pObject == TRACE_STOP_LANDSCAPE || pObject == TRACE_STOP_WATER || (IS_TRACE_STOP_OBJECT(pObject))) {
            // MoveTo
            m_pPlayerSide->PGOrderMoveTo(m_pPlayerSide->SelGroupToLogicGroup(),
                          CPoint(mx - ROBOT_MOVECELLS_PER_SIZE / 2, my - ROBOT_MOVECELLS_PER_SIZE / 2));

            CMatrixGroupObject *objs = m_pPlayerSide->GetCurGroup()->m_FirstObject;
            while (objs) {
                if (objs->GetObject() && objs->GetObject()->GetObjectType() == OBJECT_TYPE_FLYER) {
                    int param = (m_pPlayerSide->GetCurGroup()->GetObjectsCnt() -
                                 m_pPlayerSide->GetCurGroup()->GetRobotsCnt()) -
                                1;
                    if (param > 4)
                        param = 4;
                    float x = (float)g_MatrixMap->RndFloat(g_MatrixMap->m_TraceStopPos.x - param * GLOBAL_SCALE_MOVE,
                                                           g_MatrixMap->m_TraceStopPos.x + param * GLOBAL_SCALE_MOVE);
                    float y = (float)g_MatrixMap->RndFloat(g_MatrixMap->m_TraceStopPos.y - param * GLOBAL_SCALE_MOVE,
                                                           g_MatrixMap->m_TraceStopPos.y + param * GLOBAL_SCALE_MOVE);
                    ((CMatrixFlyer *)objs->GetObject())->SetTarget(D3DXVECTOR2(x, y));
                }
                objs = objs->m_NextObject;
            }
        }
    }
}

void COrderController::OnRButtonUp(const CPoint &) {
    // Do nothing
}

void COrderController::OnRButtonDouble(const CPoint &) {
    // Do nothing
}

void COrderController::OnForward(bool down) {
    DTRACE();

    auto m_Arcaded = m_pPlayerSide->m_Arcaded;

    if (!m_pPlayerSide->IsRobotMode() || !m_Arcaded)
        return;

    if (down && !m_Arcaded->AsRobot()->FindOrderLikeThat(ROT_MOVE_TO)) {
        D3DXVECTOR3 vel = m_Arcaded->AsRobot()->m_Forward * m_Arcaded->AsRobot()->GetMaxSpeed();
        float x = m_Arcaded->AsRobot()->m_PosX + vel.x;
        float y = m_Arcaded->AsRobot()->m_PosY + vel.y;
        m_Arcaded->AsRobot()->MoveTo(Float2Int(x / GLOBAL_SCALE_MOVE), Float2Int(y / GLOBAL_SCALE_MOVE));
    }
    else if (!down && m_Arcaded->AsRobot()->FindOrderLikeThat(ROT_MOVE_TO)) {
        m_Arcaded->AsRobot()->StopMoving();
    }
}

void COrderController::OnBackward(bool down) {
    DTRACE();

    auto m_Arcaded = m_pPlayerSide->m_Arcaded;

    if (!m_pPlayerSide->IsRobotMode() || !m_Arcaded)
        return;

    if (down && !m_Arcaded->AsRobot()->FindOrderLikeThat(ROT_MOVE_TO_BACK)) {
        D3DXVECTOR3 vel = m_Arcaded->AsRobot()->m_Forward * m_Arcaded->AsRobot()->GetMaxSpeed();
        float x = m_Arcaded->AsRobot()->m_PosX - vel.x;
        float y = m_Arcaded->AsRobot()->m_PosY - vel.y;
        m_Arcaded->AsRobot()->MoveToBack(Float2Int(x / GLOBAL_SCALE_MOVE), Float2Int(y / GLOBAL_SCALE_MOVE));
    }
    else if (!down && m_Arcaded->AsRobot()->FindOrderLikeThat(ROT_MOVE_TO_BACK)) {
        m_Arcaded->AsRobot()->StopMoving();
    }
}

void COrderController::OnLeft(bool down) {
    // Do nothing
}

void COrderController::OnRight(bool down) {
    // Do nothing
}
