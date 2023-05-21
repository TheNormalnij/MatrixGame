// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include <new>

#include "CStorage.hpp"
#include "Encryption.hpp"
#include "Compression.hpp"
#include "CRC32.hpp"

namespace Base {

CStorage::CStorage(CHeap *heap) : m_Heap(heap), m_Records(NULL), m_RecordsCnt(0) {}

CStorage::~CStorage() {
    Clear();
}

void CStorage::Clear(void) {
    if (m_Records) {
        for (int i = 0; i < m_RecordsCnt; ++i) {
            m_Records[i].~CStorageRecord();
        }
        HFree(m_Records, m_Heap);

        m_Records = NULL;
        m_RecordsCnt = 0;
    }
}

void CStorage::AddRecord(const CStorageRecord &sr) {
    ++m_RecordsCnt;
    m_Records = (CStorageRecord *)HAllocEx(m_Records, sizeof(CStorageRecord) * m_RecordsCnt, m_Heap);
    new(&m_Records[m_RecordsCnt - 1]) CStorageRecord(sr);
}

void CStorage::DelRecord(const wchar *table) {
    for (int i = 0; i < m_RecordsCnt; ++i) {
        if (m_Records[i].GetName() == table) {
            m_Records[i].~CStorageRecord();
            --m_RecordsCnt;
            if (m_RecordsCnt != i)
                memcpy(m_Records + i, m_Records + m_RecordsCnt, sizeof(CStorageRecord));

            break;
        }
    }
}

bool CStorage::IsTablePresent(const wchar *table) {
    for (int i = 0; i < m_RecordsCnt; ++i) {
        if (m_Records[i].GetName() == table) {
            return true;
        }
    }
    return false;
}

CDataBuf *CStorage::GetBuf(const wchar *table, const wchar *column, EStorageType st) {
    for (int i = 0; i < m_RecordsCnt; ++i) {
        if (m_Records[i].GetName() == table) {
            return m_Records[i].GetBuf(column, st);
        }
    }
    return NULL;
}

DWORD CStorage::CalcUniqID(void) {
    DWORD x = 0xFFFFFFFF;
    for (int i = 0; i < m_RecordsCnt; ++i) {
        x = m_Records[i].CalcUniqID(x);
    }
    return x;
}

void CStorage::Save(const wchar *fname, bool compression) {
    CBuf buf;
    Save(buf, compression);
    buf.SaveInFile(fname);
}

bool CStorage::Load(const wchar *fname) {
    CBuf buf;
    buf.LoadFromFile(fname);
    return Load(buf);
}

void CStorage::Save(CBuf &buf, bool compression) {
    buf.Clear();
    buf.Add<uint32_t>(0x47525453);
    buf.Add<uint32_t>(compression ? 1 : 0);  // version
    buf.Add<uint32_t>(m_RecordsCnt);         // records count

    for (int i = 0; i < m_RecordsCnt; ++i) {
        m_Records[i].Save(buf, false);
    }

    if (compression) {
        CBuf buf2;
        ZL03_Compression(buf2, buf.Buff<BYTE>() + 8, buf.Len() - 8);
        buf.SetLenNoShrink(8);
        buf.Expand(buf2.Len());
        buf.SetLenNoShrink(8 + buf2.Len());
        memcpy(buf.Buff<BYTE>() + 8, buf2.Get(), buf2.Len());
    }
}

bool CStorage::Load(CBuf &buf_in) {
    buf_in.Pointer(0);
    DWORD tag = buf_in.Get<DWORD>();
    if (tag != 0x47525453)
        return false;
    DWORD ver = buf_in.Get<DWORD>();

    if (ver > 1)
        return false;

    CBuf buf2;
    CBuf *buf = &buf_in;

    if (ver == 1) {
        // compression!
        ZL03_UnCompress(buf2, buf_in.Buff<BYTE>() + 8, buf_in.Len() - 8);
        buf = &buf2;
        buf2.Pointer(0);
    }

    return LoadRecords(*buf);
}

bool CStorage::LoadRecords(CBuf &buf) {
    if (m_Records) {
        for (int i = 0; i < m_RecordsCnt; ++i) {
            m_Records[i].~CStorageRecord();
        }
        HFree(m_Records, m_Heap);
        m_Records = NULL;
    }

    m_RecordsCnt = buf.Get<DWORD>();
    if (m_RecordsCnt == 0)
        return true;

    
    m_Records = (CStorageRecord *)HAlloc(sizeof(CStorageRecord) * m_RecordsCnt, m_Heap);

    
    if (*(buf.GetCurrent<byte>()) < 0x30) {
        for (int i = 0; i < m_RecordsCnt; ++i) {
            new (&m_Records[i]) CStorageRecord(m_Heap);
            if (!m_Records[i].LoadCacheFormat(buf)) {
                for (int j = 0; j <= i; ++j) {
                    m_Records[j].~CStorageRecord();
                }
                HFree(m_Records, m_Heap);
                m_Records = NULL;
                m_RecordsCnt = 0;
                return false;
            }
        }
    }
    else {
        for (int i = 0; i < m_RecordsCnt; ++i) {
            new (&m_Records[i]) CStorageRecord(m_Heap);
            if (!m_Records[i].Load(buf)) {
                for (int j = 0; j <= i; ++j) {
                    m_Records[j].~CStorageRecord();
                }
                HFree(m_Records, m_Heap);
                m_Records = NULL;
                m_RecordsCnt = 0;
                return false;
            }
        }
    }

    return true;
}

void CStorage::StoreBlockPar(const wchar *root, const CBlockPar &bp) {
    DTRACE();

    std::wstring root_name(root);

    CStorageRecord sr(root_name, m_Heap);
    sr.AddItem(CStorageRecordItem(L"0", ST_WCHAR));
    sr.AddItem(CStorageRecordItem(L"1", ST_WCHAR));
    sr.AddItem(CStorageRecordItem(L"2", ST_WCHAR));
    sr.AddItem(CStorageRecordItem(L"3", ST_WCHAR));
    AddRecord(sr);

    CDataBuf *propkey = GetBuf(root, L"0", ST_WCHAR);
    CDataBuf *propval = GetBuf(root, L"1", ST_WCHAR);

    int cnt = bp.ParCount();
    for (int i = 0; i < cnt; ++i) {
        propkey->AddWStr(bp.ParGetName(i));
        propval->AddWStr(bp.ParGet(i));
    }

    int uniq = 0;

    propkey = GetBuf(root, L"2", ST_WCHAR);
    propval = GetBuf(root, L"3", ST_WCHAR);
    cnt = bp.BlockCount();

    std::wstring uniq_s;
    for (int i = 0; i < cnt; ++i) {
        propkey->AddWStr(bp.BlockGetName(i));

        do {
            uniq_s = utils::format(L"%d", uniq);
            uniq++;
        }
        while (IsTablePresent(uniq_s.c_str()));

        propval->AddWStr(uniq_s);
        StoreBlockPar(uniq_s.c_str(), *bp.BlockGet(i));
    }
}

void CStorage::RestoreBlockPar(const wchar *root, CBlockPar &bp) {
    CDataBuf *propkey = GetBuf(root, L"0", ST_WCHAR);
    CDataBuf *propval = GetBuf(root, L"1", ST_WCHAR);

    for (DWORD i = 0; i < propkey->GetArraysCount(); ++i) {
        bp.ParAdd(propkey->GetAsWStr(i), propval->GetAsWStr(i));
    }

    propkey = GetBuf(root, L"2", ST_WCHAR);
    propval = GetBuf(root, L"3", ST_WCHAR);

    for (DWORD i = 0; i < propkey->GetArraysCount(); ++i) {
        CBlockPar *bp1 = bp.BlockAdd(propkey->GetAsWStr(i));
        RestoreBlockPar(propval->GetAsWStr(i).c_str(), *bp1);
    }
}

}  // namespace Base
