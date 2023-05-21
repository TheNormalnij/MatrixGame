// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CStorageRecord.hpp"
#include "CRC32.hpp"

namespace Base {

CStorageRecord::~CStorageRecord() {
    if (m_Items) {
        for (int i = 0; i < m_ItemsCount; ++i) {
            m_Items[i].ReleaseBuf(m_Heap);
            m_Items[i].~CStorageRecordItem();
        }
        HFree(m_Items, m_Heap);
    }
}

void CStorageRecord::AddItem(const CStorageRecordItem &item) {
    ++m_ItemsCount;
    m_Items = (CStorageRecordItem *)HAllocEx(m_Items, sizeof(CStorageRecordItem) * m_ItemsCount, m_Heap);
    new (&m_Items[m_ItemsCount - 1]) CStorageRecordItem(item);
    // m_Items[m_ItemsCount-1].InitBuf(m_Heap);
}

CStorageRecord::CStorageRecord(const CStorageRecord &rec) : m_Heap(rec.m_Heap), m_Name{rec.m_Name} {
    m_ItemsCount = rec.m_ItemsCount;
    m_Items = (CStorageRecordItem *)HAlloc(sizeof(CStorageRecordItem) * m_ItemsCount, m_Heap);
    for (int i = 0; i < m_ItemsCount; ++i) {
        new (&m_Items[i]) CStorageRecordItem(rec.m_Items[i]);
        m_Items[i].InitBuf(m_Heap);
    }
}

CDataBuf *CStorageRecord::GetBuf(const wchar *column, EStorageType st) {
    for (int i = 0; i < m_ItemsCount; ++i) {
        if (m_Items[i].GetName() == column)
            return m_Items[i].GetBuf(st);
    }
    return NULL;
}

void CStorageRecord::Save(CBuf &buf, bool compression) {
    buf.WStr(m_Name);
    buf.Add((DWORD)m_ItemsCount);
    for (int i = 0; i < m_ItemsCount; ++i) {
        m_Items[i].Save(buf, compression);
    }
}

DWORD CStorageRecord::CalcUniqID(DWORD xi) {
    DWORD x = CalcCRC32_Buf(xi, m_Name.c_str(), m_Name.length() * sizeof(wchar));
    for (int i = 0; i < m_ItemsCount; ++i) {
        x = m_Items[i].CalcUniqID(x);
    }
    return x;
}

bool CStorageRecord::Load(CBuf &buf) {
    if (m_Items) {
        for (int i = 0; i < m_ItemsCount; ++i) {
            m_Items[i].ReleaseBuf(m_Heap);
            m_Items[i].~CStorageRecordItem();
        }
        HFree(m_Items, m_Heap);
        m_Items = NULL;
    }

    m_Name = buf.WStr();
    m_ItemsCount = buf.Get<DWORD>();
    if (m_ItemsCount == 0)
        return true;

    m_Items = (CStorageRecordItem *)HAlloc(sizeof(CStorageRecordItem) * m_ItemsCount, m_Heap);
    for (int i = 0; i < m_ItemsCount; ++i) {
        new (&m_Items[i]) CStorageRecordItem(m_Heap);
        if (!m_Items[i].Load(buf)) {
            for (int j = 0; j <= i; ++j) {
                m_Items[j].ReleaseBuf(m_Heap);
                m_Items[j].~CStorageRecordItem();
            }
            HFree(m_Items, m_Heap);
            m_Items = NULL;
            m_ItemsCount = 0;
            return false;
        }
    }
    return true;
}

bool CStorageRecord::LoadCacheFormat(CBuf &buf) {
    if (m_Items) {
        for (int i = 0; i < m_ItemsCount; ++i) {
            m_Items[i].ReleaseBuf(m_Heap);
            m_Items[i].~CStorageRecordItem();
        }
        HFree(m_Items, m_Heap);
        m_Items = NULL;
    }
    byte unk = buf.Get<byte>();
    m_Name = buf.WStr();
    m_ItemsCount = buf.Get<DWORD>();
    if (m_ItemsCount == 0)
        return true;

    m_Items = (CStorageRecordItem *)HAlloc(sizeof(CStorageRecordItem) * m_ItemsCount, m_Heap);
    for (int i = 0; i < m_ItemsCount; ++i) {
        new (&m_Items[i]) CStorageRecordItem(m_Heap);
        if (!m_Items[i].LoadCacheFormat(buf)) {
            for (int j = 0; j <= i; ++j) {
                m_Items[j].ReleaseBuf(m_Heap);
                m_Items[j].~CStorageRecordItem();
            }
            HFree(m_Items, m_Heap);
            m_Items = NULL;
            m_ItemsCount = 0;
            return false;
        }
    }
    return true;
}

}  // namespace Base