// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "ISession.h"
#include "../transports/IServerTransport.h"
#include <string>

template <class T>
class CSession : public ISession {
    CSession(T source, IServerTransport* transport) : m_source(source) : m_transport(transport){};
    ~CSession() = default;

    T GetSource() { return m_source; };

    bool IsSource(void *source) override { return m_source == source; };
    std::string_view GetToken() override { return m_token; };

private:
    T m_source;
    IServerTransport *m_transport;
    std::string m_token;
};