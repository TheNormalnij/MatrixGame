
#pragma once

#include "uv.h"

class CSessionTCP {
public:
    CSessionTCP();

private:
    uv_tcp_t *m_client;
};