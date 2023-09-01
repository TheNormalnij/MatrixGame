// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CPlayersStore.h"

void CPlayersStore::AddPlayer(IPlayer *player) {
	if (!HasPlayer(player)) {
        m_players.push_back(player);
	}
}

void CPlayersStore::RemovePlayer(IPlayer *player) {
    m_players.remove(player);
}

bool CPlayersStore::HasPlayer(IPlayer *player) const noexcept {
	for (IPlayer* anotherPlayer : m_players) {
		if (anotherPlayer == player) {
            return true;
		}
	}
    return false;
}
