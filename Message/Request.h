#pragma once 

#include "../Library.h"

class Request {
private:    
    char* _message;
    int _messageLen;

    char* _data;
    int _dataLen;

public:
    Request();

    ~Request();

    Request(char* message, int messageLen, char* data, int dataLen);
public:
    friend bool SendRequest(Request& req, SOCKET s);

    friend bool RecvRequest(Request& req, SOCKET s);
};