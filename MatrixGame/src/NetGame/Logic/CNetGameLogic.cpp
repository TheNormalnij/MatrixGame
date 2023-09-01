// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CNetGameLogic.h"

#include "CCommandHandler.h"

CNetGameLogic::CNetGameLogic(INetworkClient *client, CServerAPI *api, CServerSync *serverSync, CMatrixMapLogic *localLogic) {
    m_pClient = client;
    m_pLocalLogic = localLogic;
    m_pNetApi = api;
    m_pServerSync = serverSync;
    m_wasBeginMessageClosed = false;
}

void CNetGameLogic::Update() {
    m_pClient->DoUpdate();

    if (!m_wasBeginMessageClosed) {
        if (!m_pLocalLogic->IsPaused()) {
            m_wasBeginMessageClosed = true;
            m_pLocalLogic->Pause(true);

            m_pNetApi->SendReady();
        }
    }

    if (m_pServerSync->GetGameStatus() != EGameStatus::RUNNING) {
        return;
    }

    if (m_pLocalLogic->IsPaused()) {
        m_pLocalLogic->Pause(false);
    }

    if (m_pServerSync->NextTick()) {
        m_pLocalLogic->SetLogicEnabed(true);
        HandleThisTickCommands();
    }
    else {
        m_pLocalLogic->SetLogicEnabed(false);
    }
}

void CNetGameLogic::HandleThisTickCommands() {
    CCommandHandler handler(m_pLocalLogic->m_Side, m_pLocalLogic->m_SideCnt);

    const auto commands = m_pServerSync->GetCommands();
    if (commands) {
        for (IGameCommand *command : *commands) {
            handler.Handle(command);
        }
    }
}
