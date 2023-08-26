// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CServerApi.h"

#include "Packets/CConnectionPacket.h"
#include "Packets/CGetGameInfoPacket.h"
#include "Packets/CReadyPacket.h"
#include "Packets/CCommandPacket.h"

void CServerAPI::SendConnect() {
    CConnectionPacket packet;
    Send(packet);
}

void CServerAPI::SendAskGameInfo() {
    CGetGameInfoPacket packet;
    Send(packet);
}

void CServerAPI::SendReady() {
    CReadyPacket packet;
    Send(packet);
}

template <class T>
void CServerAPI::SendCommands(T commands){

};

void CServerAPI::SendCommand(IGameCommand &command) {
    CCommandPacket packet(&command);
    Send(packet);
}

void CServerAPI::Send(IPacket &packet) {
    CWriteStream stream = CWriteStream();
    packet.WritePacket(&stream);

    CRequest req = CRequest(stream.GetData(), stream.GetSize());

    m_pNetClient->SendData(&req);
}
