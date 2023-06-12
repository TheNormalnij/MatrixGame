// MatrixLauncher - Launcher for SR2 planetary battle engine
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

// CApplication.h : Defines CApplication interface

#pragma once

class CApplication {
public:
    virtual void StartLocalGame(wchar_t *map) = 0;
    virtual void StartLauncher() = 0;
};