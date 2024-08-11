#include "Request.h"

#include "../Library.h"

Request::Request() {
    _message = nullptr;
    _messageLen = 0;

    _data = nullptr;
    _dataLen = 0;
}

Request::~Request() {
    delete[] _message;
    delete[] _data;
}

Request::Request(char* message, int messageLen, char* data, int dataLen) {
    _message = message;
    _messageLen = messageLen;

    _data = data;
    _dataLen = dataLen;
}

bool SendRequest(Request& req, SOCKET s) {
    if (SendAll(s, req._message, req._messageLen) == false) {
        return false;
    }   
    
    if (req._dataLen != 0) {
        if (SendAll(s, req._data, req._dataLen) == false) {
            return false;
        }
    }   

    return true;
}

bool RecvRequest(Request& req, SOCKET s) {
    if (RecvAll(s, req._message, req._messageLen) == false) {
        return false;
    }
    
    if (RecvAll(s, req._data, req._dataLen) == false) {
        return false;
    }
    // std::cout << "second: " << req._data << "\n";
    return true;
}