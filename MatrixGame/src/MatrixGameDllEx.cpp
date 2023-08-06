// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included
// New intereace implementation


#include "MatrixGameDllEx.hpp"
#include "Interface/CInterface.h"
#include <string_view>
#include "RangersInterface/RangersInterfaceInternal.h"
#include "Pack.hpp"
#include "NetGame/Logic/NetGame.h"
#include "MatrixFormGame.hpp"

// TODO more map info
MATRIXGAMEDLL_API void __cdecl InterateMaps(void predicate(const wchar_t *name)) {
#ifndef MAXEXP_EXPORTS
    CHeap heap = CHeap{};
    CPackFile robotPack = CPackFile(&heap, L"DATA\\robots.pkg");
    robotPack.OpenPacketFile();

    CHsFolder *mapsFolder = robotPack.GetRootFolder()->GetFolderEx("Matrix\\Map\\");

    static auto _predicate = predicate;
    static wchar_t buff[100];
    mapsFolder->ForFiles([](const char *name) {
        std::string_view nameStr = std::string_view(name);

        if (nameStr.ends_with(".CMAP")) {
            std::mbstowcs(buff, name, 100);
            _predicate(buff);
        }
    });

    robotPack.ClosePacketFile();
#endif
}

MATRIXGAMEDLL_API int __cdecl RunStandalone(HINSTANCE hinst, wchar *map, SMatrixSettings *set,
                                               SMatrixTextParams *textParams, SRobotGameState *rgs) {
    g_RangersInterface = RangersInterfaceInternal::getInstance()->getSMGDIntervace();
    RangersInterfaceInternal::getInstance()->LoadResources(set->m_Lang);

    CGame game{};

    uint32_t seed = (unsigned)time(NULL);

    game.Init(hinst, NULL, map, seed, set, textParams);

    CFormMatrixGame formgame;

    game.RunGameLoop(&formgame);

    game.SaveResult(rgs);
    game.SafeFree();

    g_RangersInterface = nullptr;

    return g_ExitState;
}

MATRIXGAMEDLL_API int __cdecl ConnectNetGame(HINSTANCE hinst, char *host, SMatrixSettings *set, SRobotGameState *rgs) {
    g_RangersInterface = RangersInterfaceInternal::getInstance()->getSMGDIntervace();
    RangersInterfaceInternal::getInstance()->LoadResources(set->m_Lang);

    SMatrixTextParams textParams;

    textParams.lossText = L"";
    textParams.planetName = L"";
    textParams.startText = L"";
    textParams.winText = L"";

    CNetGame netGame(hinst, set);

    const bool status = netGame.ConnectGame(host);

    g_RangersInterface = nullptr;

    if (!status) {
        return 99;
    }

    return g_ExitState;
}
