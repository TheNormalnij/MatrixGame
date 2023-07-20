// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CServerCore.h"

int main(int argc, char *argv[]) {
    CServerCore server;

    server.StartServer("0.0.0.0", 22173);
}