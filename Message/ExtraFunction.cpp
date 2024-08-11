#include "../Library.h"

#include "ExtraFunction.h"

// Receive all message
bool RecvAll(SOCKET s, char* buf, int &len) {
    int bytesleft = len; 
    int total = 0;
    int iResult;

    while (total < len) {
        iResult = recv(s, buf + total, bytesleft, 0);
        if (iResult <= 0) {
            break;
        }
        total += iResult;
        bytesleft -= iResult;
    }

    // std::cout << "last buf: " << buf << "\n";
    // std::cout << "bytes send: " << total << "\n";
    len = total;
    buf[len] = '\0';    
    return true;
}

// Send all message
bool SendAll(SOCKET s, char* buf, int &len) {
    int total = 0; 
    int bytesleft = len; 
    int iResult;
    
    while (total < len) {
        // std::cout << " buf: " << buf << "\n";
        iResult = send(s, buf + total, bytesleft, 0);
        if (iResult <= 0) {
            break;
        }
        total += iResult;
        bytesleft -= iResult;
    }
    
    // std::cout << "last buf: " << buf << "\n";
    // std::cout << "bytes send: " << total << "\n";
    len = total;
    return true;
}

char* ConvertStringToChar(std::string s) {
    char* c = new char[s.size() + 1];
    strcpy(c, s.c_str());
    return c;
}