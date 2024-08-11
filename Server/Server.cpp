    // g++ mainServer.cpp Server/*.cpp Server/Function/CaptureScreen.cpp Message/*.cpp -o main -lws2_32 -lgdi32 -luser32; .\main

#include "../Library.h"
#include "../Message/Request.h"
#include "../Message/Response.h"
#include "../Message/ExtraFunction.h"
#include "Function\ControlApp.h"
#include "Function\ListProcess.h"
#include "Function\ListDirectory.h"
#include "Function\KeyLog.h"
#include "Function\CaptureScreen.h"
#include "Server.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")

Server::Server() {
    std::cout << "Server is running" << "\n";
        
    WSADATA wsaData;
    struct addrinfo *result = NULL;
    struct addrinfo hints;
    int iResult;
    
    // Initialize Winsock 
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        std::cerr << "Server WSAS failed to start : " << iResult << "\n";
    }

    // List wsa info
    std::cout << "wsa version: " << wsaData.wVersion << "\n";
    // std::cout << "wsa highversion: " << wsaData.wHighVersion << "\n";
    std::cout << "wsa description: " << wsaData.szDescription << "\n";
    std::cout << "wsa system status: " << wsaData.szSystemStatus << "\n";
    std::cout << "wsa max sockets: " << wsaData.iMaxSockets << "\n";

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        std::cerr << "Get server address failed with error: " << iResult << "\n";
        WSACleanup();
        exit(1);
    } 
    
    // Create a SOCKET for the server to listen for client connections.
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        std::cerr << "Setup listen socket failed with error: " << WSAGetLastError() << "\n";
        exit(1);
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "Bind failed with error: " << WSAGetLastError() << "\n";
        exit(1);
    }

    char host[NI_MAXHOST];
    struct hostent *host_entry;
    int hostname = SOCKET_ERROR;
    hostname = gethostname(host, NI_MAXHOST);
    if (hostname == SOCKET_ERROR) {
        std::cerr << "Get host name failed with error: " << WSAGetLastError() << "\n";
        exit(1);
    }
    host_entry = gethostbyname(host);
    if (host_entry == NULL) {
        std::cerr << "Get host by name failed with error: " << WSAGetLastError() << "\n";
        exit(1);
    }
    std::cout << "Host name: " << host_entry->h_name << "\n";
    std::cout << "IP address: " << inet_ntoa(*(struct in_addr *)*host_entry->h_addr_list) << "\n";
    
    freeaddrinfo(result);

    // Listen on a client socket
    std::cout << "Waiting request from client...\n";
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "Listen failed with error: " << WSAGetLastError() << "\n";
        exit(1);
    }
}

Server::~Server() {
    closesocket(ClientSocket);
    closesocket(ListenSocket);
    WSACleanup();
}

void Server::ListApp() {
    std::vector <std::string> listApp = ListInstalledApp();

    std::stringstream ss;
    int i = 1;
    for (const auto& app: listApp) {
        ss << i << ". ";
        ss << app << "\n";
        i++;
    }
    std::string appString = ss.str();
    char* buffer = new char[appString.size() + 1];
    strcpy(buffer, appString.c_str());

    Response res(buffer, strlen(buffer));
    if (SendResponse(res, ClientSocket) == false) {
        std::cerr << "Send response failed\n";
        exit(1);    
    } else {
        std::cout << "Send response successfull\n";
    }
}

void Server::StartApp(std::string name) {
    bool error = startApplication(name);
    if (error == false) {
        char* buffer = ConvertStringToChar("Can't start app");
        
        Response res(buffer, strlen(buffer));
        if (SendResponse(res, ClientSocket) == false) {
            std::cerr << "Send response failed\n";
            exit(1);    
        } else {
            std::cout << "Send response successfull\n";
        }
    } else {
        char* buffer = ConvertStringToChar("Start app successfully");
        Response res(buffer, strlen(buffer));
        if (SendResponse(res, ClientSocket) == false) {
            std::cerr << "Send response failed\n";
            exit(1);    
        } else {
            std::cout << "Send response successfull\n";
        }
    }
    
}

void Server::StopApp(std::string name) {
    bool error = stopProcessApp(name);
    if (error == false) {
        char* buffer = ConvertStringToChar("Error! Can't stop app");
        Response res(buffer, strlen(buffer));
        if (SendResponse(res, ClientSocket) == false) {
            std::cerr << "Send response failed\n";
            exit(1);    
        } else {
            std::cout << "Send response successfull\n";
        }
    } else {
        char* buffer = ConvertStringToChar("Stop app successfully");
        Response res(buffer, strlen(buffer));
        if (SendResponse(res, ClientSocket) == false) {
            std::cerr << "Send response failed\n";
            exit(1);    
        } else {
            std::cout << "Send response successfull\n";
        }
    }
}

void Server::ListProcess() {
    std::vector <std::string> listProcess = ListProcessApp();

    std::stringstream ss;
    int i = 1;
    for (const auto& app: listProcess) {
        ss << i << ". ";
        ss << app << "\n";
        i++;
    }
    std::string ProcessString = ss.str();
    char* buffer = new char[ProcessString.size() + 1];
    strcpy(buffer, ProcessString.c_str());

    Response res(buffer, strlen(buffer));
    if (SendResponse(res, ClientSocket) == false) {
        std::cerr << "Send response failed\n";
        exit(1);    
    } else {
        std::cout << "Send response successfull\n";
    }
}

void Server::ListDirectory(std::string path) {
    std::string list = listDir(path);
    std::stringstream ss;
    ss << list;
    std::string listString = ss.str();

    char* buffer = new char[listString.size() + 1];
    strcpy(buffer, listString.c_str());
    // std::cout << listString.size() << "\n";
    Response res(buffer, strlen(buffer));
    if (SendResponse(res, ClientSocket) == false) {
        std::cerr << "Send response failed" << "\n";
        exit(1);
    }  else {
        std::cout << "Send response successfull" << "\n";
    }
}

void Server::KeyLog() {
    bool keyDown[256] = {false};
    while (true) {
        std::string keyLog = getKeyLog(keyDown);
        
        std::stringstream ss;
        ss << keyLog;
        std::string listKey = ss.str();

        char* buffer = new char[listKey.size() + 1];
        strcpy(buffer, listKey.c_str());
   
        int iResult = send(ClientSocket, buffer, strlen(buffer), 0);
        if (iResult == SOCKET_ERROR) {
            std::cerr << "\nSend response failed" << "\n";
            exit(1);
        }

        // check escape
        buffer[iResult] = '\0';
        std::cout << buffer;
        if (strcmp(buffer, "Escape") == 0) {
            std::cout << "\nEscape pressed. Ending receive loop.\n";
            break;
        }
    }
    std::cout << "\nEnd keylog\n";
}

void Server::CaptureScreen() {
    // Capture screen in server
    if (!CaptureAnImage()) {
        std::cerr << "Capture screen failed\n";
        exit(1);
    }
    
    // Send image to client
    FILE *picture = fopen(SaveScreenShotServer, "rb");
    if (picture == NULL) {
        std::cerr << "Cannot open file\n";
        exit(1);
    }
    fseek(picture, 0, SEEK_END);
    int size = ftell(picture);
    fseek(picture, 0, SEEK_SET);
    char *buffer = new char[DEFAULT_BUFLEN];
    std::cout << "Sending image to client...\n";

    // response to client
    int remainBytes = size;
    // std::cout << "remainBytes: " << remainBytes << "\n";
    while (remainBytes > 0) {
        int bytesToSend = std::min(DEFAULT_BUFLEN, remainBytes);
        // std::cout << "bytesToSend: " << bytesToSend << "\n";
        fread(buffer, bytesToSend, 1, picture);

        int iResult = send(ClientSocket, buffer, bytesToSend, 0);
        // Response res(buffer, bytesToSend);
        if (iResult == 0) {
            std::cerr << "Send respoinse failed" << "\n";
            exit(1);
        }

        remainBytes -= bytesToSend;
    }

    std::cout << "Send picture successfully" << "\n";
    std::cout << "size of image: " << size << "\n";
  

    delete[] buffer;
}

void Server::Process(char* request, char* data) {
    // std::cout << "request: " << request << "\n";
    // std::cout << "data: " << data << "\n";
    if (strcmp(request, "Capture Screen") == 0) {
        std::cout << "Running capture screen in server" << "\n";
        CaptureScreen();
    } else if (strcmp(request, "List App") == 0) {
        std::cout << "Running list installed app in server" << "\n";
        ListApp();
    } else if (strcmp(request, "List Process") == 0) {
        std::cout << "Running list processing in server" << "\n";
        ListProcess();
    } else if (strcmp(request, "Stop App") == 0) {
        std::cout << "Running stop app in server" << "\n";
        StopApp(data);
    } else if (strcmp(request, "Start App") == 0) {
        std::cout << "Running start app in server" << "\n";
        StartApp(data);
    } else if (strcmp(request, "List Directory") == 0) {
        std::cout << "Running list directory in server" << "\n";
        ListDirectory(data);
    } else if (strcmp(request, "Key Log") == 0) {
        std::cout << "Running key log in server" << "\n";
        KeyLog();
    } else {
        exit(1);
    }
}

void Server::Start() {
    ClientSocket = accept(ListenSocket, NULL, NULL);
    int iResult;
    if (ClientSocket == INVALID_SOCKET) {
        std::cerr << "Accept failed with error: " << WSAGetLastError() << "\n";
        exit(1);
    }
    std::cout << "Client connected\n";

    // Receive request from client
    char recvbuf[DEFAULT_BUFLEN] = "\0";
    int recvbufSize = DEFAULT_BUFLEN;
    char data[DEFAULT_BUFLEN] = "\0";
    int dataSize = DEFAULT_BUFLEN;

    iResult = recv(ClientSocket, recvbuf, recvbufSize, 0);
    int iResult1 = 1;
    // std::cout << recvbuf << "\n";
    if (strcmp(recvbuf, "Start App") == 0 || strcmp(recvbuf, "Stop App") == 0 || strcmp(recvbuf, "List Directory") == 0) {
        iResult1 = recv(ClientSocket, data, dataSize, 0);
    }
  
    if (iResult == 0 || iResult1 == 0) {
        std::cerr << "Receive request failed\n";
        exit(1);
    } else {
        std::cout << "Receive request successfull\n";
        // std::cout << "Request is: " << recvbuf << "\n";
        // std::cout << "Data is: " << data << "\n";
        Process(recvbuf, data);
    }

    // iResult = shutdown(ListenSocket, SD_SEND);
    // if (iResult == SOCKET_ERROR) {
    //     std::cerr << "Shutdown failed with error: " << WSAGetLastError() << "\n";
    // }
    
    // iResult = shutdown(ClientSocket, SD_SEND);
    // if (iResult == SOCKET_ERROR) {
    //     std::cerr << "Shutdown failed with error: " << WSAGetLastError() << "\n";
    // }
}


