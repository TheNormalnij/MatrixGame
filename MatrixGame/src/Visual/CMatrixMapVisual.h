// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "MatrixCursor.hpp"
#include "MatrixCamera.hpp"
#include "MatrixDebugInfo.hpp"
#include "CAlwaysDrawStorage.h"
#include "IMatrixMapVisual.h"
#include "CMatrixVisiCalc.h"

#include "MatrixMap.hpp"

#include <array>

class CMatrixMapVisual : public IMatrixMapVisual {
public:
    CMatrixMapVisual(CMatrixMap *map);
    ~CMatrixMapVisual() = default;

    void SetSkyColor(unsigned int color) noexcept { m_SkyColor = color; };
    void SetShadowColor(unsigned int color) { m_ShadowColor = color; };
    void SetAmbientObjColor(unsigned int color) { m_AmbientColorObj = color; };
    unsigned int GetShadowColor() const noexcept { return m_ShadowColor; };
    void SetBiasTer(float bias) noexcept { m_BiasTer = bias; };
    void SetBiasWater(float bias) noexcept { m_BiasWater = bias; };
    void SetBiasRobot(float bias) noexcept { m_BiasRobots = bias; };

    // IMatrixMapVisual
    SSkyTex *GetSkyTex(int index) noexcept override { return &m_SkyTex[index]; };
    void ClearResources() override;
    void CreatePoolDefaultResources() override;
    float GetRobotBias() const noexcept override { return m_BiasRobots; };
    unsigned long GetCurrentFrame() const noexcept override { return m_CurFrame; };
    unsigned int GetAmbientColorObj() const noexcept override { return m_AmbientColorObj; };

    void BeforeDraw() override;
    void Draw() override;

    void BeforeDrawAllLandscape() noexcept override { BeforeDrawLandscape(true); };
    void BeforeDrawAllLandscapeSurfaces() noexcept override { BeforeDrawLandscapeSurfaces(true); };
    void DrawAllLandscape() noexcept override { DrawLandscape(true); };
    void DrawAllLandscapeSurfaces() noexcept override { DrawLandscapeSurfaces(true); };

private:
    const D3DXMATRIX &GetIdentityMatrix(void) const { return m_Map->GetIdentityMatrix(); };
    auto GetVisibleCalculator() const noexcept { return m_Map->GetVisibleCalculator(); };

    void DrawSky();
    void DrawFog();

    void UpdateTraceObject();

    void BeforeDrawLandscape(bool all = false);
    void BeforeDrawLandscapeSurfaces(bool all = false);

    void DrawLandscape(bool all = false);
    void DrawLandscapeSurfaces(bool all = false);
    void DrawObjects(void);
    void DrawWater(void);
    void DrawShadowsProjFast(void);
    void DrawShadows(void);
    void DrawEffects(void);

private:
    CMatrixMap *m_Map;
    CMatrixCursor *m_Cursor;
    CMatrixCamera *m_Camera;
    CMatrixDebugInfo *m_DI;
    CAlwaysDrawStorage *m_AlwaysDrawStorage;

    unsigned int m_CurFrame;

    unsigned int m_SkyColor;
    std::array<SSkyTex, 4> m_SkyTex;

    float m_BiasTer;
    float m_BiasWater;
    float m_BiasRobots;
    
    unsigned int m_ShadowColor;
    unsigned int m_AmbientColorObj;
    D3D_VB m_ShadowVB;
};
