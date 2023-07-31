// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "ClientTCP.h"

void CClientTCP::Connect(std::string_view adress, net_client_connect_cb callback) {
    callback(this, true, "");
}

void CClientTCP::Close(std::function<net_client_close_cb> callback) {}

void CClientTCP::SendData(CRequest *req) {}
