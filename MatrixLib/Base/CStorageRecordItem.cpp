// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CStorageRecordItem.hpp"
#include "Compression.hpp"
#include "CRC32.hpp"

namespace Base {

    void CStorageRecordItem::InitBuf(CHeap *heap) {
        ReleaseBuf(heap);
        m_Buf = HNew(heap) CDataBuf(heap, m_Type);
    }

    void CStorageRecordItem::ReleaseBuf(CHeap *heap) {
        if (m_Buf) {
            HDelete(CDataBuf, m_Buf, heap);
            m_Buf = NULL;
        }
    }

    DWORD CStorageRecordItem::CalcUniqID(DWORD xi) {
        DWORD x = CalcCRC32_Buf(xi, m_Name.c_str(), m_Name.length() * sizeof(wchar));
        x = CalcCRC32_Buf(x, m_Buf->Get(), m_Buf->Len());
        x = CalcCRC32_Buf(x, &m_Type, sizeof(m_Type));
        return x;
    }

    void CStorageRecordItem::Save(CBuf &buf, bool compression) {
        ASSERT(m_Buf);
        m_Buf->Compact();
        buf.WStr(m_Name);

        if (compression) {
            buf.Dword(m_Type | ST_COMPRESSED);
            CBuf cb(buf.m_Heap);
            ZL03_Compression(cb, (BYTE *)m_Buf->Get(), m_Buf->Len());

            buf.Dword(cb.Len());
            buf.BufAdd(cb.Get(), cb.Len());
        }
        else {
            buf.Dword(m_Type);
            buf.Dword(m_Buf->Len());
            buf.BufAdd(m_Buf->Get(), m_Buf->Len());
        }
    }

    bool CStorageRecordItem::Load(CBuf &buf) {
        ASSERT(m_Buf);

        m_Name = buf.WStr();
        m_Type = (EStorageType)buf.Dword();
        DWORD sz = buf.Dword();

        if (m_Type & ST_COMPRESSED) {
            m_Type = (EStorageType)(m_Type & ST_COMPRESSED);
            if (0 == ZL03_UnCompress(*m_Buf, buf.Buff<BYTE>() + buf.Pointer(), sz))
                return false;
        }
        else {
            m_Buf->Clear();
            m_Buf->Expand(sz);
            buf.BufGet(m_Buf->Get(), sz);
        }

        return true;
    }

    bool CStorageRecordItem::LoadCacheFormat(CBuf &buf) {
        ASSERT(m_Buf);

        uint8_t unk = buf.Byte();
        m_Name = buf.WStr();
        m_Type = (EStorageType)buf.Dword();
        DWORD sz = buf.Dword();

        if (m_Type & ST_COMPRESSED) {
            m_Type = (EStorageType)(m_Type & ST_COMPRESSED);
            if (0 == ZL03_UnCompress(*m_Buf, buf.Buff<BYTE>() + buf.Pointer(), sz))
                return false;
        }
        else {
            m_Buf->Clear();
            m_Buf->Expand(sz);
            buf.BufGet(m_Buf->Get(), sz);
        }

        return true;
    }

}  // namespace Base