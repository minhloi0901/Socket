#pragma once

#include "../Library.h"

class Response {
private:
    char* _message;
    int _messageLen;

public:
    Response();

    ~Response();

    Response(char* message, int messageLen);

public:
    void print();

    friend bool SendResponse(Response& res, SOCKET s);

    friend bool RecvResponse(Response& res, SOCKET s);
};