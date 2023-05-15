// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "Base.hpp"
#include "CStorageRecord.hpp"

namespace Base {

class CStorage : public CMain {
    CHeap *m_Heap;

    CStorageRecord *m_Records;
    int m_RecordsCnt;

public:
    CStorage(CHeap *heap = NULL);
    ~CStorage();

    void Clear(void);

    void AddRecord(const CStorageRecord &sr);
    void DelRecord(const wchar *table);

    CDataBuf *GetBuf(const wchar *table, const wchar *column, EStorageType st);
    bool IsTablePresent(const wchar *table);

    DWORD CalcUniqID(void);

    void Save(const wchar *fname, bool compression = false);
    bool Load(const wchar *fname);

    void Save(CBuf &buf, bool compression = false);
    bool Load(CBuf &buf);

    void StoreBlockPar(const wchar *root, const CBlockPar &bp);
    void RestoreBlockPar(const wchar *root, CBlockPar &bp);

private:
    bool LoadRecords(CBuf &buf);
};

}  // namespace Base
