// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "MatrixAIGroup.h"
#include "../MatrixMap.hpp"
#include "../MatrixRobot.hpp"
#include "../MatrixSide.hpp"
#include "MatrixLogicSlot.h"
#include "MatrixRule.h"
#include "../MatrixFlyer.hpp"

CMatrixGroup::CMatrixGroup() {
    m_NextGroup = NULL;
    m_PrevGroup = NULL;
    m_FirstObject = NULL;
    m_LastObject = NULL;
    m_ObjectsCnt = 0;
    m_RobotsCnt = 0;
    m_FlyersCnt = 0;
    m_BuildingsCnt = 0;
    m_Team = -1;
    // m_Tactics       = NULL;
    m_Id = 0;
    m_GroupPosition = D3DXVECTOR3(0, 0, 0);
    m_GroupSpeed = 0;
    m_SimpleTimer = -1;
}

CMatrixGroup::~CMatrixGroup() {
    DTRACE();
    RemoveAll();
    // if(m_Tactics)
    //    HDelete(CMatrixTactics, m_Tactics, g_MatrixHeap);
}

void CMatrixGroup::AddObject(CMatrixMapStatic *object, int team) {
    CMatrixGroupObject *objects = m_FirstObject;
    while (objects) {
        if (objects->GetObject() == object)
            return;
        objects = objects->m_NextObject;
    }
    CMatrixGroupObject *g_object = HNew(g_MatrixHeap) CMatrixGroupObject;
    g_object->SetObject(object);
    g_object->SetParentGroup(this);
    g_object->SetTeam(team);
    LIST_ADD(g_object, m_FirstObject, m_LastObject, m_PrevObject, m_NextObject);
    if (object->GetObjectType() == OBJECT_TYPE_ROBOTAI) {
        m_RobotsCnt++;
    }
    else if (object->GetObjectType() == OBJECT_TYPE_FLYER) {
        m_FlyersCnt++;
    }
    else if (object->GetObjectType() == OBJECT_TYPE_BUILDING) {
        m_BuildingsCnt++;
    }
    m_ObjectsCnt++;
    CalcGroupPosition();
    CalcGroupSpeed();
}

void CMatrixGroup::RemoveObject(CMatrixMapStatic *object) {
    CMatrixGroupObject *g_object = m_FirstObject;
    while (g_object) {
        if (g_object->GetObject() == object) {
            LIST_DEL(g_object, m_FirstObject, m_LastObject, m_PrevObject, m_NextObject);
            HDelete(CMatrixGroupObject, g_object, g_MatrixHeap);
            if (object->GetObjectType() == OBJECT_TYPE_ROBOTAI) {
                m_RobotsCnt--;
            }
            else if (object->GetObjectType() == OBJECT_TYPE_FLYER) {
                m_FlyersCnt--;
            }
            else if (object->GetObjectType() == OBJECT_TYPE_BUILDING) {
                m_BuildingsCnt--;
            }
            m_ObjectsCnt--;
            CalcGroupPosition();
            CalcGroupSpeed();

            return;
        }
        g_object = g_object->m_NextObject;
    }
}

void CMatrixGroup::RemoveAll() {
    DTRACE();
    CMatrixGroupObject *Objects = m_FirstObject;

    while (Objects != NULL) {
        if (Objects->m_NextObject)
            Objects = Objects->m_NextObject;
        else {
            HDelete(CMatrixGroupObject, Objects, g_MatrixHeap);
            Objects = NULL;
            m_FirstObject = NULL;
            m_LastObject = NULL;
        }

        if (Objects)
            HDelete(CMatrixGroupObject, Objects->m_PrevObject, g_MatrixHeap);
    }

    m_RobotsCnt = 0;
    m_FlyersCnt = 0;
    m_BuildingsCnt = 0;
    m_ObjectsCnt = 0;
}

void CMatrixGroup::FindNearObjects(CMatrixGroupObject *fn_object) {
    CMatrixGroupObject *gr_objects = m_FirstObject;

    while (gr_objects) {
        CMatrixMapStatic *ai_object = gr_objects->GetObject();

        if (ai_object->GetObjectType() == OBJECT_TYPE_ROBOTAI) {
            if (((CMatrixRobotAI *)ai_object)->GetGroup() != 0 || (fn_object->GetTeam() != gr_objects->GetTeam())) {
                gr_objects = gr_objects->m_NextObject;
                continue;
            }
        }
        else if (ai_object->GetObjectType() == OBJECT_TYPE_FLYER) {
            if (((CMatrixFlyer *)ai_object)->m_Group != 0 || (fn_object->GetTeam() != gr_objects->GetTeam())) {
                gr_objects = gr_objects->m_NextObject;
                continue;
            }
        }

        // check r

        D3DXVECTOR2 fr(0, 0);
        D3DXVECTOR2 ar(0, 0);
        int *ai_gr = NULL, *fn_gr = NULL;

        if (fn_object->GetObject()->GetObjectType() == OBJECT_TYPE_ROBOTAI) {
            fn_gr = ((CMatrixRobotAI *)fn_object->GetObject())->GetGroupP();
            fr = D3DXVECTOR2(((CMatrixRobotAI *)fn_object->GetObject())->m_PosX,
                             ((CMatrixRobotAI *)fn_object->GetObject())->m_PosY);
        }
        else if (fn_object->GetObject()->GetObjectType() == OBJECT_TYPE_FLYER) {
            fn_gr = &((CMatrixFlyer *)fn_object->GetObject())->m_Group;
            fr = D3DXVECTOR2(((CMatrixFlyer *)fn_object->GetObject())->GetPos().x,
                             ((CMatrixFlyer *)fn_object->GetObject())->GetPos().y);
        }

        if (ai_object->GetObjectType() == OBJECT_TYPE_ROBOTAI) {
            ai_gr = ((CMatrixRobotAI *)ai_object)->GetGroupP();
            ar = D3DXVECTOR2(((CMatrixRobotAI *)ai_object)->m_PosX, ((CMatrixRobotAI *)ai_object)->m_PosY);
        }
        else if (ai_object->GetObjectType() == OBJECT_TYPE_FLYER) {
            ai_gr = &((CMatrixFlyer *)ai_object)->m_Group;
            ar = D3DXVECTOR2(((CMatrixFlyer *)ai_object)->GetPos().x, ((CMatrixFlyer *)ai_object)->GetPos().y);
        }

        const auto tmp = ar - fr;
        if (D3DXVec2LengthSq(&tmp) > 0 && D3DXVec2LengthSq(&tmp) <= DIF_GROUP_R * DIF_GROUP_R) {
            *ai_gr = *fn_gr;
            // ai_object->m_Group = fn_object->GetObject()->m_Group;

            FindNearObjects(gr_objects);
        }
        //
        gr_objects = gr_objects->m_NextObject;
    }
}

void CMatrixGroup::LogicTakt(CMatrixSideUnit *side) {
    if (m_SimpleTimer >= JUST_PERIOD || m_SimpleTimer == -1) {
        CalcGroupPosition();
        CalcGroupSpeed();
        m_SimpleTimer = 0;
    }

    m_SimpleTimer++;

    // if(m_Tactics){
    //    m_Tactics->LogicTakt(side, this);
    //}
}

void CMatrixGroup::CalcGroupPosition() {
    CMatrixGroupObject *objects = m_FirstObject;

    D3DXVECTOR3 pos(0, 0, 0);
    while (objects) {
        CMatrixMapStatic *ai_object = objects->GetObject();
        if (ai_object->GetObjectType() == OBJECT_TYPE_ROBOTAI) {
            pos.x += ((CMatrixRobotAI *)ai_object)->m_PosX;
            pos.y += ((CMatrixRobotAI *)ai_object)->m_PosY;
        }
        else if (ai_object->GetObjectType() == OBJECT_TYPE_FLYER) {
            pos.x += ((CMatrixFlyer *)ai_object)->GetPos().x;
            pos.y += ((CMatrixFlyer *)ai_object)->GetPos().y;
        }

        objects = objects->m_NextObject;
    }
    if (m_ObjectsCnt > 1) {
        pos.x /= m_ObjectsCnt;
        pos.y /= m_ObjectsCnt;
    }
    m_GroupPosition = pos;
}

void CMatrixGroup::CalcGroupSpeed() {
    CMatrixGroupObject *objects = m_FirstObject;
    float lowest_speed = 0;

    while (objects) {
        CMatrixMapStatic *ai_object = objects->GetObject();
        if (ai_object->GetObjectType() == OBJECT_TYPE_ROBOTAI &&
            ((CMatrixRobotAI *)ai_object)->GetMaxSpeed() < lowest_speed &&
            ((CMatrixRobotAI *)ai_object)->m_CurrState == ROBOT_SUCCESSFULLY_BUILD /* || lowest_speed == 0*/) {
            lowest_speed = ((CMatrixRobotAI *)ai_object)->GetMaxSpeed();
        }
        else if (ai_object->GetObjectType() == OBJECT_TYPE_FLYER &&
                 ((CMatrixFlyer *)ai_object)->GetSpeed() < lowest_speed) {
            lowest_speed = ((CMatrixFlyer *)ai_object)->GetSpeed();
        }

        objects = objects->m_NextObject;
    }
    m_GroupSpeed = lowest_speed;
}

void CMatrixGroup::RemoveBuildings() {
    if (m_BuildingsCnt) {
        CMatrixGroupObject *go = m_FirstObject;

        while (go) {
            if (go->GetObject()->GetObjectType() == OBJECT_TYPE_BUILDING) {
                CMatrixGroupObject *go2 = go->m_NextObject;
                LIST_DEL(go, m_FirstObject, m_LastObject, m_PrevObject, m_NextObject);
                HDelete(CMatrixGroupObject, go, g_MatrixHeap);
                m_BuildingsCnt--;
                m_ObjectsCnt--;
                go = go2;
                continue;
            }

            go = go->m_NextObject;
        }
    }
}

void CMatrixGroup::SortFlyers() {
    CMatrixGroupObject *go = m_FirstObject;

    while (go->GetObject()->GetObjectType() == OBJECT_TYPE_FLYER) {
        LIST_DEL(go, m_FirstObject, m_LastObject, m_PrevObject, m_NextObject);
        LIST_ADD(go, m_FirstObject, m_LastObject, m_PrevObject, m_NextObject);
        go = m_FirstObject;
    }
}

CMatrixMapStatic *CMatrixGroup::GetObjectByN(int num) {
    if (num > m_ObjectsCnt) {
        return NULL;
    }
    CMatrixGroupObject *go = m_FirstObject;

    while (go && num--) {
        go = go->m_NextObject;
    }

    if (go) {
        return go->GetObject();
    }
    return NULL;
}

bool CMatrixGroup::RemoveObject(int num) {
    CMatrixMapStatic *o = GetObjectByN(num);
    if (o) {
        RemoveObject(o);
        return true;
    }
    return false;
}

bool CMatrixGroup::FindObject(CMatrixMapStatic *object) {
    DTRACE();
    CMatrixGroupObject *objects = m_FirstObject;

    while (objects) {
        if (objects->GetObject() == object)
            return true;
        objects = objects->m_NextObject;
    }
    return false;
}

int CMatrixGroup::GetBombersCnt() {
    DTRACE();

    int cnt = 0;
    CMatrixGroupObject *objects = m_FirstObject;

    while (objects) {
        if (objects->GetObject()->IsLiveRobot()) {
            if (objects->GetObject()->AsRobot()->FindWeapon(WEAPON_BIGBOOM)) {
                cnt++;
            }
        }
        objects = objects->m_NextObject;
    }

    return cnt;
}

int CMatrixGroup::GetRepairsCnt() {
    DTRACE();

    int cnt = 0;
    CMatrixGroupObject *objects = m_FirstObject;

    while (objects) {
        if (objects->GetObject()->IsLiveRobot()) {
            if (objects->GetObject()->AsRobot()->FindWeapon(WEAPON_REPAIR)) {
                cnt++;
            }
        }
        objects = objects->m_NextObject;
    }

    return cnt;
}
