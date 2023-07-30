
#pragma once

#include "IClient.h"
#include "../Packets/IPacketHandler.h"

class CClientTCP : public INetworkClient {
public:
    CClientTCP();
    ~CClientTCP();

private:
    IPacketHandler *m_pPacketHandler;
};