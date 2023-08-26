// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2023, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "MatrixMap.hpp"
#include "MatrixRobot.hpp"
#include "MatrixObjectCannon.hpp"

// TODO: разбросать функции в cpp

inline bool PrepareBreakOrder(CMatrixMapStatic *robot) {
    return !(((CMatrixRobotAI *)robot)->GetEnv()->m_OrderNoBreak = !robot->AsRobot()->CanBreakOrder());
}

inline bool IsLiveUnit(CMatrixMapStatic *obj) {
    if (obj->GetObjectType() == OBJECT_TYPE_ROBOTAI)
        return obj->AsRobot()->m_CurrState !=
               ROBOT_DIP;  // && (obj->AsRobot()->GetSide()!=PLAYER_SIDE || !obj->AsRobot()->IsSelected()) &&
                           // (g_MatrixMap->GetPlayerSide()->GetArcadedObject() != obj);
    else if (obj->GetObjectType() == OBJECT_TYPE_CANNON)
        return obj->AsCannon()->m_CurrState != CANNON_DIP && obj->AsCannon()->m_CurrState != CANNON_UNDER_CONSTRUCTION;
    else
        return false;
}

inline CPoint GetMapPos(CMatrixMapStatic *obj) {
    switch (obj->GetObjectType()) {
        case OBJECT_TYPE_ROBOTAI:
            return CPoint(obj->AsRobot()->GetMapPosX(), obj->AsRobot()->GetMapPosY());
        case OBJECT_TYPE_CANNON:
            return CPoint(int(((CMatrixCannon *)obj)->m_Pos.x / GLOBAL_SCALE_MOVE),
                          int(((CMatrixCannon *)obj)->m_Pos.y / GLOBAL_SCALE_MOVE));
        case OBJECT_TYPE_BUILDING:
            return CPoint(int(((CMatrixBuilding *)obj)->m_Pos.x / GLOBAL_SCALE_MOVE),
                          int(((CMatrixBuilding *)obj)->m_Pos.y / GLOBAL_SCALE_MOVE));
    }
    if (obj->GetObjectType() == OBJECT_TYPE_MAPOBJECT && obj->IsSpecial()) {
        return CPoint(int(((CMatrixMapObject *)obj)->GetGeoCenter().x / GLOBAL_SCALE_MOVE),
                      int(((CMatrixBuilding *)obj)->GetGeoCenter().y / GLOBAL_SCALE_MOVE));
    }
    ERROR_E;
}

inline D3DXVECTOR2 GetWorldPos(CMatrixMapStatic *obj) {
    switch (obj->GetObjectType()) {
        case OBJECT_TYPE_ROBOTAI:
            return D3DXVECTOR2(obj->AsRobot()->m_PosX, obj->AsRobot()->m_PosY);
        case OBJECT_TYPE_CANNON:
            return obj->AsCannon()->m_Pos;
        case OBJECT_TYPE_BUILDING:
            return obj->AsBuilding()->m_Pos;
    }
    ERROR_E;
}

inline int RobotPlace(CMatrixRobotAI *robot) {
    return robot->GetEnv()->m_Place;
    //    if(IsToPlace(robot,robot->GetEnv()->m_Place)) return robot->GetEnv()->m_Place;
    //    else return -1;
}

// Движется ли робот к назначенному месту
inline bool IsToPlace(CMatrixRobotAI *robot, int place) {
    if (place < 0)
        return false;

    SMatrixPlace *pl = GetPlacePtr(place);

    CPoint tp;
    if (robot->GetMoveToCoords(tp)) {
        if (robot->FindOrderLikeThat(ROT_CAPTURE_FACTORY))
            return false;
        else if ((pl->m_Pos.x == tp.x) && (pl->m_Pos.y == tp.y))
            return true;
        else if (robot->GetReturnCoords(tp) && (pl->m_Pos.x == tp.x) && (pl->m_Pos.y == tp.y))
            return true;
        else
            return false;
    }
    else {
        if (robot->GetReturnCoords(tp) && (pl->m_Pos.x == tp.x) && (pl->m_Pos.y == tp.y))
            return true;

        return (robot->GetMapPosX() == pl->m_Pos.x) && (robot->GetMapPosY() == pl->m_Pos.y);
        // return fabs(robot->m_PosX-GLOBAL_SCALE_MOVE*(ROBOT_MOVECELLS_PER_SIZE>>1)-GLOBAL_SCALE_MOVE*pl->m_Pos.x)<0.9f
        // && fabs(robot->m_PosY-GLOBAL_SCALE_MOVE*(ROBOT_MOVECELLS_PER_SIZE>>1)-GLOBAL_SCALE_MOVE*pl->m_Pos.y)<0.9f;
    }
}

// Если робот стоит на месте
inline bool IsInPlace(CMatrixRobotAI *robot, int place) {
    SMatrixPlace *pl = GetPlacePtr(place);

    if (pl == NULL)
        return false;

    if (robot->FindOrderLikeThat(ROT_MOVE_TO))
        return false;

    return (robot->GetMapPosX() == pl->m_Pos.x) && (robot->GetMapPosY() == pl->m_Pos.y);
}

// Если робот стоит на месте
inline bool IsInPlace(CMatrixRobotAI *robot) {
    return IsInPlace(robot, RobotPlace(robot));
}

inline int CannonPlace(CMatrixCannon *cannon) {
    return cannon->m_Place;
}

inline int ObjPlace(CMatrixMapStatic *obj) {
    if (obj->IsRobot())
        return RobotPlace(obj->AsRobot());
    else if (obj->IsCannon())
        return CannonPlace(obj->AsCannon());
    ERROR_E;
}

inline SMatrixPlace *ObjPlacePtr(CMatrixMapStatic *obj) {
    int i = ObjPlace(obj);
    if (i >= 0)
        return GetPlacePtr(i);
    else
        return NULL;
}

inline dword ObjPlaceData(CMatrixMapStatic *obj) {
    int i = ObjPlace(obj);
    if (i >= 0)
        return GetPlacePtr(i)->m_Data;
    else
        return 0;
}

inline void ObjPlaceData(CMatrixMapStatic *obj, dword data) {
    int i = ObjPlace(obj);
    if (i >= 0)
        GetPlacePtr(i)->m_Data = data;
}

inline bool CanMove(byte movetype, CMatrixRobotAI *robot) {
    return !(movetype & (1 << (robot->m_Unit[0].u1.s1.m_Kind - 1)));
}

inline int GetDesRegion(CMatrixRobotAI *robot) {
    SMatrixPlace *pl = ObjPlacePtr(robot);
    if (!pl)
        return -1;
    return pl->m_Region;
    /*    CPoint tp;

        if(robot->GetMoveToCoords(tp)) return GetRegion(tp);
        else return GetRegion(robot->GetMapPosX(),robot->GetMapPosY());*/
}

inline int GetRegion(const CPoint &tp) {
    return g_MatrixMap->GetRegion(tp);
}

inline int GetRegion(int x, int y) {
    return g_MatrixMap->GetRegion(x, y);
}

inline int GetRegion(CMatrixMapStatic *obj) {
    return GetRegion(GetMapPos(obj));
}

inline D3DXVECTOR3 PointOfAim(CMatrixMapStatic *obj) {
    D3DXVECTOR3 p;

    if (obj->GetObjectType() == OBJECT_TYPE_ROBOTAI) {
        p = obj->GetGeoCenter();
        p.z += 5.0f;
    }
    else if (obj->GetObjectType() == OBJECT_TYPE_CANNON) {
        p = obj->GetGeoCenter();
        p.z += 5.0f;

        /*        p.x=((CMatrixCannon *)(obj))->m_Pos.x;
                p.y=((CMatrixCannon *)(obj))->m_Pos.y;
                p.z=g_MatrixMap->GetZ(p.x,p.y)+25.0f;*/
    }
    else if (obj->GetObjectType() == OBJECT_TYPE_BUILDING) {
        p = obj->GetGeoCenter();
        p.z = g_MatrixMap->GetZ(p.x, p.y) + 20.0f;
    }
    else
        ASSERT(0);

    return p;
}

inline bool PLIsToPlace(CMatrixRobotAI *robot) {
    CPoint tp, ptp;
    if (robot->GetEnv()->m_Place >= 0) {
        ptp = GetPlacePtr(robot->GetEnv()->m_Place)->m_Pos;
    }
    else if (robot->GetEnv()->m_PlaceAdd.x >= 0) {
        ptp = robot->GetEnv()->m_PlaceAdd;
    }
    else
        return false;

    if (robot->GetMoveToCoords(tp)) {
        if (robot->FindOrderLikeThat(ROT_CAPTURE_FACTORY))
            return false;
        else if ((ptp.x == tp.x) && (ptp.y == tp.y))
            return true;
        else if (robot->GetReturnCoords(tp) && (ptp.x == tp.x) && (ptp.y == tp.y))
            return true;
        else
            return false;
    }
    else {
        if (robot->GetReturnCoords(tp) && (ptp.x == tp.x) && (ptp.y == tp.y))
            return true;

        return (robot->GetMapPosX() == ptp.x) && (robot->GetMapPosY() == ptp.y);
    }
}

inline CPoint PLPlacePos(CMatrixRobotAI *robot) {
    if (robot->GetEnv()->m_Place >= 0) {
        return GetPlacePtr(robot->GetEnv()->m_Place)->m_Pos;
    }
    else if (robot->GetEnv()->m_PlaceAdd.x >= 0) {
        return robot->GetEnv()->m_PlaceAdd;
    }
    else
        return CPoint(-1, -1);
}

inline int GetObjTeam(CMatrixMapStatic *robot) {
    return ((CMatrixRobotAI *)(robot))->GetTeam();
}
inline CInfo *GetEnv(CMatrixMapStatic *robot) {
    return ((CMatrixRobotAI *)(robot))->GetEnv();
}
inline int GetGroupLogic(CMatrixMapStatic *robot) {
    return ((CMatrixRobotAI *)(robot))->GetGroupLogic();
}
inline float Dist2(D3DXVECTOR2 p1, D3DXVECTOR2 p2) {
    return POW2(p1.x - p2.x) + POW2(p1.y - p2.y);
}
inline bool CanChangePlace(CMatrixRobotAI *robot) {
    return (g_MatrixMap->GetTime() - robot->GetEnv()->m_PlaceNotFound) > 2000;
}