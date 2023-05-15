// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "CDataBuf.hpp"

namespace Base {

    class CStorageRecordItem : public CMain {
        std::wstring m_Name;
        EStorageType m_Type;

        CDataBuf *m_Buf;

    public:
        CStorageRecordItem(const CStorageRecordItem &item) : m_Name(item.m_Name), m_Type(item.m_Type), m_Buf(NULL) {}
        CStorageRecordItem(const std::wstring &name, EStorageType type) : m_Name(name), m_Type(type), m_Buf(NULL) {}
        CStorageRecordItem(CHeap *heap) : m_Name{}, m_Buf(NULL) { InitBuf(heap); }
        ~CStorageRecordItem() = default;

        void InitBuf(CHeap *heap);
        void ReleaseBuf(CHeap *heap);
        const std::wstring &GetName(void) const { return m_Name; }

        CDataBuf *GetBuf(EStorageType st) { return (st == m_Type) ? m_Buf : NULL; }

        DWORD CalcUniqID(DWORD x);

        void Save(CBuf &buf, bool compression);
        bool Load(CBuf &buf);
        bool LoadCacheFormat(CBuf &buf);
    };

}  // namespace Base