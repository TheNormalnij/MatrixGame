// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "transports/ITransportHandler.h"
#include "packet_handlers/IPacketHandler.h"
#include "CGamePacketHandlerFactory.h"
#include <net/PacketEnums.h>

class CGamePacketHandler : public ITransportHandler {
public:
    CServerPacketHandler(IGame *game) : m_game(game){};
    ~CServaerPacketHandler() = default;

    void HandlePacket(char *data, size_t len) override;

private:
    IGame *m_game;
    CGamePacketHandlerFactory m_handlerFactory;
    IPacketHandler* GetPacketHandlerById(EGamePacketType packetId) const;
};
