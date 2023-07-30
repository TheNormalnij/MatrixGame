
#pragma once

#include <string_view>
#include <shared/net/CRequest.h>

typedef void(*net_client_connect_cb)(INetworkClient *client, bool success, std::string_view error);

class INetworkClient {
public:
    virtual ~INetworkClient() = default;
    virtual void Connect(std::string_view adress, net_client_connect_cb callback) = 0;
    virtual void SendData(CRequest *req) = 0
};