// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#include "CGameNetwork.h"
#include <shared/net/CWriteStream.h>
#include <shared/net/PacketEnums.h>
#include "net/packets/CCommandsPacket.h"
#include "net/packets/CChangeGameStatePacket.h"
#include "net/packets/CGameInfoPacket.h"

struct SMiltictasRequestSendStatus {
    size_t sendetCount = 0;
    size_t targetCount = 0;
    CWriteStream *stream;
};

void CGameNetwork::SendTickCommands(size_t tick, std::list<IGameCommand *> &commands) {
    CCommandsPacket packet = CCommandsPacket(commands);
    Broadcast(packet);
}

void CGameNetwork::SendGameStatusChanged(EGameStatus status) {
    CChangeGameStatePacket packet = CChangeGameStatePacket(status);
    Broadcast(packet);
}

void CGameNetwork::SendGameInfo(ISession *session, std::string_view mapName) {
    CGameInfoPacket packet(mapName);
    Unicast(session, packet);
}

void CGameNetwork::Broadcast(IPacket &packet) {
    CWriteStream *stream = new CWriteStream();
    packet.WritePacket(stream);

    CRequest *req = new CRequest(stream->GetData(), stream->GetSize());

    SMiltictasRequestSendStatus *status = new SMiltictasRequestSendStatus();
    status->stream = stream;
    req->SetCustomData(status);

    req->SetCallback([](CRequest *req, bool success) {
        SMiltictasRequestSendStatus *status = (SMiltictasRequestSendStatus *)req->GetCustomData();
        status->sendetCount++;
        if (status->sendetCount >= status->targetCount) {
            delete req;
            delete status->stream;
            delete status;
        }
    });

    for (ISession *session : m_sessionStore->GetSessions()) {
        session->SendData(req);
        status->targetCount++;
    }
}

void CGameNetwork::Unicast(ISession *session, IPacket &packet) {
    CWriteStream *stream = new CWriteStream();
    packet.WritePacket(stream);

    CRequest *req = new CRequest(stream->GetData(), stream->GetSize());

    req->SetCustomData(stream);

    req->SetCallback([](CRequest *req, bool status) {
        delete (CWriteStream *)req->GetCustomData();
        delete req;
    });

    session->SendData(req);
}
