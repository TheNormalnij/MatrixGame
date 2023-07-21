
#pragma once

#include "uv.h"
#include "ISession.h"

class CSessionTCP : public ISession {
public:
    CSessionTCP() = default;
    ~CSessionTCP() = default;

    void Send(char *data, size_t len);

    uv_tcp_t *GetHandler() const { return m_handler; };
    void RegisterInEventLoop(uv_loop_t* loop);

    // Free itself after operation
    void CloseAndDestroy();

private:
    uv_tcp_t m_handler;
};