// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CServerMatrixGame.h"

CServerMatrixGame::CServerMatrixGame() {
    m_currentTick = 0;
}

void CServerMatrixGame::GameStart(){

};

void CServerMatrixGame::GameStop(){

}

void CServerMatrixGame::DoTick() {
    if (m_currentStatus != EGameStatus::RUNNING) {
        return;
    }

    // Increase tick counter
    m_currentTick++;
    SendCurrentTickCommands();
}

void CServerMatrixGame::SendCurrentTickCommands() {
    //const 
}

void CServerMatrixGame::HandleCommand(IGameCommand command) {
    m_commandLog.PushCommand(m_currentTick + 1, command);
};
