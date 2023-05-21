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
            buf.Add((DWORD)(m_Type | ST_COMPRESSED));
            CBuf cb;
            ZL03_Compression(cb, (BYTE *)m_Buf->Get(), m_Buf->Len());

            buf.Add((DWORD)cb.Len());
            buf.Add(cb.Get(), cb.Len());
        }
        else {
            buf.Add((DWORD)m_Type);
            buf.Add((DWORD)m_Buf->Len());
            buf.Add(m_Buf->Get(), m_Buf->Len());
        }
    }

    bool CStorageRecordItem::Load(CBuf &buf) {
        ASSERT(m_Buf);

        m_Name = buf.WStr();
        m_Type = (EStorageType)buf.Get<DWORD>();
        DWORD sz = buf.Get<DWORD>();

        if (m_Type & ST_COMPRESSED) {
            m_Type = (EStorageType)(m_Type & ST_COMPRESSED);
            if (0 == ZL03_UnCompress(*m_Buf, buf.Buff<BYTE>() + buf.Pointer(), sz))
                return false;
        }
        else {
            m_Buf->Clear();
            m_Buf->Expand(sz);
            buf.Get(m_Buf->Get(), sz);
        }

        return true;
    }

    bool CStorageRecordItem::LoadCacheFormat(CBuf &buf) {
        ASSERT(m_Buf);

        uint8_t unk = buf.Get<byte>();
        m_Name = buf.WStr();
        m_Type = (EStorageType)buf.Get<DWORD>();
        DWORD sz = buf.Get<DWORD>();

        if (m_Type & ST_COMPRESSED) {
            m_Type = (EStorageType)(m_Type & ST_COMPRESSED);
            if (0 == ZL03_UnCompress(*m_Buf, buf.Buff<BYTE>() + buf.Pointer(), sz))
                return false;
        }
        else {
            m_Buf->Clear();
            m_Buf->Expand(sz);
            buf.Get(m_Buf->Get(), sz);
        }

        return true;
    }

}  // namespace Base