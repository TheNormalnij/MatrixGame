// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

class RangersGame {
public:
    RangersGame() = default;
    ~RangersGame() = default;

    void SetProgressBar(float val);

    void Begin();
    void DoTact();

    float GetMusicVolume();
    void SetMusicVolume(float volume);
};