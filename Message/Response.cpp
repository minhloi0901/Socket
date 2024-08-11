#include "Response.h"

Response::Response() {
    _message = nullptr;
    _messageLen = 0;
}

Response::~Response() {
    delete[] _message;
}

Response::Response(char* message, int messageLen) {
    _message = message;
    _messageLen = messageLen;
}

void Response::print() {
    std::cout << _message << "\n";
}

bool SendResponse(Response& res, SOCKET s) {
    if (SendAll(s, res._message, res._messageLen) == false) {
        return false;
    }
    
    return true;
}

bool RecvResponse(Response& res, SOCKET s) {
    if (RecvAll(s, res._message, res._messageLen) == false) {
        return false;
    }

    return true;
}