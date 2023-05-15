// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include <windows.h>
#include "CBuf.hpp"

#undef ZEXPORT
#define ZEXPORT __cdecl

#include "zlib.h"

namespace Base {

int ZL03_UnCompress(CBuf &out, BYTE *in, int inlen);

void ZL03_Compression(CBuf &out, BYTE *in, int inlen);

int ZL01_UnCompress(CBuf &out, BYTE *in, int inlen);

int UnCompress(CBuf &out, BYTE *in, int inlen);
}