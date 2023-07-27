// MatrixServer - Multiplayer server for SR2 PB game
// Copyright (C) 2023, Uladzislau "TheNormalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included
 
#pragma once

class CRequest;

typedef void (*request_cb)(CRequest *req, bool success);

class CRequest {
public:
    CRequest(char *data, unsigned long len) : m_data(data), m_len(len){};
    ~CRequest() = default;

    void SetCallback(request_cb cb) { m_callBack = cb; };

    void CallCallback(bool success) {
        if (m_callBack)
            m_callBack(this, success);
    }

    char *GetData() const { return m_data; };
    unsigned long GetSize() const { return m_len; };


    void SetCustomData(void *data) { m_customData = data; };
    void *GetCustomData() const { return m_customData; };

private:
    unsigned long m_len;
    char *m_data;
    request_cb m_callBack = nullptr;
    void *m_customData = nullptr;
};