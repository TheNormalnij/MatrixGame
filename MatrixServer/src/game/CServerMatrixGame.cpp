// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CServerMatrixGame.h"

CServerMatrixGame::CServerMatrixGame() {
    m_currentStatus = EGameStatus::WAIT_PLAYERS;
    m_currentTick = 0;
}

void CServerMatrixGame::GameStart(){
    if (m_currentStatus != EGameStatus::WAIT_PLAYERS_READY) {
        return;
    }

    m_currentStatus = EGameStatus::RUNNING;
    m_net->SendGameStart();
};

void CServerMatrixGame::GameStop(){
    if (m_currentStatus != EGameStatus::RUNNING) {
        return;
    }

    m_net->SendGameStop();
}

void CServerMatrixGame::DoTick() {
    // Skip if not running
    if (m_currentStatus != EGameStatus::RUNNING) {
        return;
    }

    // Increase tick counter
    m_currentTick++;

    // Send current tick commands
    m_net->SendTickCommands(m_currentTick, m_commandLog.GetTickCommands(m_currentTick));
}

void CServerMatrixGame::HandleCommand(IGameCommand command) {
    m_commandLog.PushCommand(m_currentTick + 1, command);
};
