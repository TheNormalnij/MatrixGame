// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once
class CMatrixRobotAI;
class CMatrixMapStatic;

class CEnemy: public CMain{
    public:
        DWORD               m_EnemyKind;
        CMatrixMapStatic*   m_Enemy;

        CEnemy*             m_NextEnemy;
        CEnemy*             m_PrevEnemy;

        int                 m_DelSlowly;

    public:
        CEnemy();
        ~CEnemy();

        void SetEnemy(CMatrixMapStatic* ms)                                         { m_Enemy = ms; }
        CMatrixMapStatic* GetEnemy()                                                { return m_Enemy; }

        void SetKind(DWORD kind)                                                    { m_EnemyKind |= kind; }
        DWORD GetKind()                                                             { return m_EnemyKind; }

        void ClassifyEnemy(CMatrixMapStatic* relTo);   
};

class CInfo : public CMain {
    public:
        int                 m_EnemyCnt;

        DWORD               m_EnemyKind;
        CMatrixMapStatic*   m_Target;
        CMatrixMapStatic*   m_TargetAttack;
        CMatrixMapStatic*   m_TargetLast;               // ��������� ����, ���������� ����� ��� �����������, ����� ���������� ����
        int                 m_TargetChange;             // ����� ����� ���� ���������
        int                 m_TargetChangeRepair;       // ����� ����� ���� ���������
        float               m_TargetAngle;

        int                 m_Place;                    // ����� � ������� ����� ��� � ������� ���� �����
        CPoint              m_PlaceAdd;                 // �������������� �����, ����������� �������
        int                 m_PlaceNotFound;            // ����� �� �������. ����� ������ ������ ����� ������������ �����
        DWORD               m_OrderNoBreak;             // ����������� ������ ������� ������ �������� (bool variable)
        int                 m_LastFire;                 // �����, ����� ����� ��������� ��� ���������
        int                 m_LastHitTarget;            // �����, ����� ����� ��������� ��� ����� � ����
        int                 m_LastHitEnemy;             // �����, ����� ����� ��������� ��� ����� �� �����
        int                 m_LastHitFriendly;          // �����, ����� ����� ��������� ��� ����� � ������������� ������

        int                 m_BadPlaceCnt;
        int                 m_BadPlace[8];              // ������ ������ ����, ���� ���������� �� �����

        int                 m_BadCoordCnt;
        CPoint              m_BadCoord[16];             // ������ ������ ���������, ����� ����� ���������� ������� �� ��� ���������� ������������

        int                 m_IgnoreCnt;                // ������ �����, �� ������� �� �������� ��������
        CMatrixMapStatic*   m_Ignore[16];
        int                 m_IgnoreTime[16];

        CEnemy*             m_FirstEnemy;
        CEnemy*             m_LastEnemy;

    public:
        CInfo();
        ~CInfo();
    
        void Clear(void);

        int TargetType(void)                            { if(m_Target==NULL) return 0; else if(SearchEnemy(m_Target)) return 1; else return 2; }

        void RemoveAllBuilding(CMatrixMapStatic * skip=NULL);
        void RemoveAllSlowely(void);
    
        void RemoveFromList(CMatrixMapStatic* ms);
        void RemoveFromList(CEnemy * enemy);
        void RemoveFromListSlowly(CMatrixMapStatic* ms);
        void AddToList(CMatrixMapStatic* ms);
        void AddToListSlowly(CMatrixMapStatic* ms);
        CEnemy * SearchEnemy(CMatrixMapStatic* ms);
        CMatrixMapStatic* GetEnemyByKind(DWORD kind);    
        int GetEnemyCnt()                                                               { return m_EnemyCnt; }
        void KillEnemyByKind(DWORD kind)                                                { m_EnemyKind = kind; }

        void AddBadPlace(int place);
        bool IsBadPlace(int place);

        void AddBadCoord(CPoint & coord);
        bool IsBadCoord(CPoint & coord);

        void AddIgnore(CMatrixMapStatic* ms);
        bool IsIgnore(CMatrixMapStatic* ms);
        void DelIgnore(CMatrixMapStatic* ms);

        void Reset();
};
