// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "../Net/CServerApi.h"
#include "../Net/CServerSync.h"
#include "INetGameLogic.h"
#include "MatrixLogic.hpp"

class CNetGameLogic : public INetGameLogic {
public:
    CNetGameLogic(INetworkClient *client, CServerAPI *api, CServerSync *serverSync, CMatrixMapLogic *localLogic);
    ~CNetGameLogic() = default;

    void Update() override;

private:
    CMatrixMapLogic *m_pLocalLogic;
    CServerAPI *m_pNetApi;
    CServerSync *m_pServerSync;
    INetworkClient *m_pClient;
    bool m_wasBeginMessageClosed;
};