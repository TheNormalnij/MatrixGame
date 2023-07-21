
#include "CSessionTCP.h"

CSessionTCP::CSessionTCP() {
    m_handler.data = this;
}

void CSessionTCP::RegisterInEventLoop(uv_loop_t *loop) {
    uv_tcp_init(loop, m_handler);
}

void CSessionTCP::CloseAndDestroy() {
    uv_close((uv_handle_t *)m_handler, []((uv_handle_t *)handle) {
        delete (CSessionTCP *)handle->data;
    });
}
