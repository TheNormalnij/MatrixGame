// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "ServerApi.h"

#include "Packets/ConnectionPacket.h"
#include "Packets/CGetGameInfoPacketHandler.h"

void CServerAPI::SendConnect() {
    CConnectionPacket packet;
    Send(packet);
}

void CServerAPI::SendAskGameInfo() {
    CGetGameInfoPacketHandler packet;
    Send(packet);
}

void CServerAPI::Send(IPacket &packet) {
    CWriteStream stream = CWriteStream();
    packet.WritePacket(&stream);

    CRequest req = CRequest(stream.GetData(), stream.GetSize());

    m_pNetClient->SendData(&req);
}

template <class T>
void CServerAPI::SendCommands(T commands){
    
};
