// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "RangersCacheDataLoader.h"
#include "Encryption.hpp"
#include "Compression.hpp"

const uint32_t CACHE_FILE_KEY = 0xEA8F3F37;

enum class eBlockParKeyType : uint8_t {
    DATA_KEY = 1,
    ROOT_KEY = 2,
};

void CRangersCacheDataLoader::Load(const std::wstring_view filePath, Base::CBlockPar &out) {
    Base::CBuf buf = Base::CBuf();
    buf.LoadFromFile(filePath.data());

    Load(buf, out);
}

void CRangersCacheDataLoader::Load(Base::CBuf &buf_in, Base::CBlockPar &out) {
    buf_in.Pointer(0);

    // Ќужно еще делать проверку по этому значению
    uint32_t crc32 = buf_in.Get<DWORD>();
    uint32_t fileKey = buf_in.Get<DWORD>();

    // ќтклонение от начального значени€
    size_t skipBytes = 8;
    if (crc32 == 0xB589C17F && fileKey == 0xCCE115B0) {
        // CacheData.dat from game case
        crc32 = buf_in.Get<DWORD>();
        fileKey = buf_in.Get<DWORD>();
        skipBytes += 8;
    }

    Base::CBuf clearBuf = Base::CBuf();

    GetClearBuffer(buf_in.Buff<char>() + skipBytes, buf_in.Len() - skipBytes, clearBuf, fileKey);

    LoadData(clearBuf, &out);
}

void CRangersCacheDataLoader::GetClearBuffer(const char *in, size_t inSize, Base::CBuf &clearBuf, uint32_t fileKey) {
    Base::CBuf decryptedBuf = Base::CBuf();

    size_t decryptedSize = inSize;
    decryptedBuf.Expand(decryptedSize);
    decryptedBuf.SetLenNoShrink(decryptedSize);

    // Decode
    encodeOrDecodeData(in, decryptedBuf.Buff<char>(), inSize, CACHE_FILE_KEY ^ fileKey);

    // Decompress
    UnCompress(clearBuf, decryptedBuf.Buff<BYTE>(), decryptedBuf.Len());
    clearBuf.Pointer(0);
}

void CRangersCacheDataLoader::LoadData(Base::CBuf &buf, Base::CBlockPar *out) {
    const uint32_t valuesCount = buf.Get<DWORD>();
    for (uint32_t i = 0; i < valuesCount; i++) {
        const eBlockParKeyType keyType = (eBlockParKeyType)buf.Get<uint8_t>();
        const std::wstring key = buf.WStr();
        if (keyType == eBlockParKeyType::DATA_KEY) {
            std::wstring value = buf.WStr();
            out->ParAdd(key, value);
        }
        else {
            Base::CBlockPar *block = out->BlockAdd(key);
            LoadData(buf, block);
        }
    }
}