// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

class NetConnection {
public:
    NetConnection(std::string_view host) : m_host(host){};
    ~NetConnection() = default;

    void Connect();
    void Reconnect();
    void Disconnect();
    void SendPacket();
    void ReadPacket();

private:
    std::string m_host;
};