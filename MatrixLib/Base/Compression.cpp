// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "Compression.hpp"

namespace Base {

int ZL03_UnCompress(CBuf &out, BYTE *in, int inlen) {
    out.Clear();

    if (inlen < 8)
        return 0;
    if (*(in + 0) != 'Z' || *(in + 1) != 'L' || *(in + 2) != '0' || *(in + 3) != '3')
        return 0;

    int cnt = *(int *)(in + 4);

    int optr = 0;
    int iptr = 8;

    for (; cnt > 0; --cnt) {
        out.Pointer(optr);
        out.Expand(65000);

        DWORD len = out.Len() - optr;
        int szb = *(DWORD *)(in + iptr);
        if (uncompress(out.Buff<BYTE>() + optr, &len, in + iptr + 4, szb) != Z_OK) {
            out.Clear();
            return 0;
        }

        iptr += szb + 4;
        optr += len;
        out.SetLenNoShrink(optr);
    }

    out.SetLenNoShrink(optr);
    return optr;
}

void ZL03_Compression(CBuf &out, BYTE *in, int inlen) {
    out.Clear();
    out.Add((uint8_t)'Z');
    out.Add((uint8_t)'L');
    out.Add((uint8_t)'0');
    out.Add((uint8_t)'3');
    out.Add((uint16_t)0);  // will be updated. blocks count

    int cnt = 0;

    int ptro = out.Pointer();
    int ptri = 0;

    int szi = inlen;

    while (szi >= 65000) {
        out.Pointer(ptro);
        out.Expand(65536 * 2);

        DWORD len = out.Len() - ptro + 4;
        int res = compress2(out.Buff<BYTE>() + ptro + 4, &len, in + ptri, 65000, Z_BEST_COMPRESSION);
        if (res != Z_OK) {
            debugbreak();
        }
        ptri += 65000;

        *(DWORD *)(out.Buff<BYTE>() + ptro) = len;
        ptro += len + 4;
        out.SetLenNoShrink(ptro);

        szi -= 65000;
        ++cnt;
    }

    if (szi > 0) {
        out.Pointer(ptro);
        out.Expand(szi * 2);

        DWORD len = szi;
        compress2(out.Buff<BYTE>() + ptro + 4, &len, in + ptri, szi, Z_BEST_COMPRESSION);

        *(DWORD *)(out.Buff<BYTE>() + ptro) = len;
        ptro += len + 4;

        ++cnt;
    }

    out.SetLenNoShrink(ptro);
    *(DWORD *)(out.Buff<BYTE>() + 4) = cnt;
}

int ZL01_UnCompress(CBuf &out, BYTE *in, int inlen) {
    out.Clear();

    if (*(in + 0) != 'Z' || *(in + 1) != 'L' || *(in + 2) != '0' || *(in + 3) != '1')
        return 0;

    int size = *(int *)(in + 4);

    out.Expand(size);

    DWORD len = size;
    const int res = uncompress(out.Buff<BYTE>(), &len, in + 8, inlen - 8);

    if (res != Z_OK) {
        out.Clear();
        return 0;
    }

    return len;
}

int UnCompress(CBuf &out, BYTE *in, int inlen) {
    if (*(in + 0) != 'Z' || *(in + 1) != 'L' || *(in + 2) != '0')
        return 0;

    if (*(in + 3) == '3')
        return ZL03_UnCompress(out, in, inlen);

    if (*(in + 3) == '1')
        return ZL01_UnCompress(out, in, inlen);

    return 0;
}

}