// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

struct SSkyTex {
    CTextureManaged *tex;
    float u0, v0, u1, v1;
};

enum {
    SKY_FORE,
    SKY_BACK,
    SKY_LEFT,
    SKY_RITE,
};

class IMatrixMapVisual {
public:
    // Remove this shit later
    //
    virtual SSkyTex *GetSkyTex(int index) noexcept = 0;
    virtual float GetRobotBias() const noexcept = 0;
    virtual unsigned long GetCurrentFrame() const noexcept = 0;
    virtual unsigned int GetAmbientColorObj() const noexcept = 0;

    // Real interface
    virtual void ClearResources() = 0;
    virtual void CreatePoolDefaultResources() = 0;
    virtual void BeforeDraw() = 0;
    virtual void Draw() = 0;

    // For map
    virtual void BeforeDrawAllLandscape() noexcept = 0;
    virtual void BeforeDrawAllLandscapeSurfaces() noexcept = 0;
    virtual void DrawAllLandscape() noexcept = 0;
    virtual void DrawAllLandscapeSurfaces() noexcept = 0;
};
