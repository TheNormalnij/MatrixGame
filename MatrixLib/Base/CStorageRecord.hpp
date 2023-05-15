// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "CStorageRecordItem.hpp"

namespace Base {

class CStorageRecord : public CMain {
    CHeap *m_Heap;

    std::wstring m_Name;  // record name (table name)
    CStorageRecordItem *m_Items;
    int m_ItemsCount;

public:
    CStorageRecord(const CStorageRecord &rec);
    CStorageRecord(std::wstring name, CHeap *heap = NULL)
      : m_Heap(heap), m_Name(name), m_Items(NULL), m_ItemsCount(0) {}
    CStorageRecord(CHeap *heap) : m_Heap(heap), m_Name{}, m_Items(NULL), m_ItemsCount(0) {}
    ~CStorageRecord();

    const std::wstring &GetName(void) const { return m_Name; }

    void AddItem(const CStorageRecordItem &item);
    CDataBuf *GetBuf(const wchar *column, EStorageType st);

    DWORD CalcUniqID(DWORD x);

    void Save(CBuf &buf, bool compression);
    bool Load(CBuf &buf);
    bool LoadCacheFormat(CBuf &buf);
};

}  // namespace Base