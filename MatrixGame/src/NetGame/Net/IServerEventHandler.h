// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

class IServerEventHandler {
public:
    void OnPlayerConnected(unsigned char side, char *name) = 0;
    void OnPlayerDisconnected(unsigned char side) = 0;
    void OnGetCommand() = 0;
    void OnGetMapInfo(char *mapName) = 0;
};