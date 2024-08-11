#include "../Library.h"
#include "../Message/Request.h"
#include "../Message/Response.h"
#include "../Message/ExtraFunction.h"

#include "Client.h"

// g++ mainClient.cpp Client/*.cpp Message/*.cpp -o mainClient -lws2_32 -lgdi32 -luser32; .\mainClient

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

Client::Client() {
    std::cout << "Client is running" << "\n";
    WSADATA wsaData;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;

    // Initialize Winsock
    int iResult;
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        std::cerr << "Client WSAS failed to start : " << iResult << "\n";
        exit(1);
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

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
        std::cerr << "Get client by name failed with error: " << WSAGetLastError() << "\n";
        exit(1);
    }
    std::cout << "Client name: " << host_entry->h_name << "\n";
    ClientIP = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));
    std::cout << "Client IP: " << ClientIP << "\n";

    // Resolve the server address and port
    iResult = getaddrinfo(ClientIP, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        std::cerr << "Client getaddrinfo  failed with error: " << iResult << "\n";
        exit(1);   
    }

    // Attempt to connect to an address until one succeeds
    for(ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ServerSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ServerSocket == INVALID_SOCKET) {
            std::cerr << "Socket failed with error: " << WSAGetLastError() << "\n";
            exit(1);
        }

        // Connect to server.
        iResult = connect(ServerSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ServerSocket);
            ServerSocket = INVALID_SOCKET;
            continue;
        } 
        break;
    }

    freeaddrinfo(result);

    if (ServerSocket == INVALID_SOCKET) {
        std::cerr << "Unable to connect to server!" << "\n";
        exit(1);
    } 
}


Client::~Client() {
    closesocket(ServerSocket);
    WSACleanup();
}

void Client::ListApp() {
    char* request = ConvertStringToChar("List App");
    char* nulString = ConvertStringToChar("\0");
    Request req(request, strlen(request), nulString, 0);
    bool iResult = SendRequest(req, ServerSocket);
    if (iResult == false) {
        std::cerr << "Send request to server failed with error: " << WSAGetLastError() << "\n";
        exit(1);
    } else {
        std::cout << "Send request to server successfully" << "\n";
    } 

    // Receive data from server
    char recvbuf[DEFAULT_BUFLEN];
    Response res(recvbuf, DEFAULT_BUFLEN);
    if (RecvResponse(res, ServerSocket) == 0) {
        std::cerr << "Receive response list app failed with error: " << WSAGetLastError() << "\n";
        exit(1);
    } else {
        std::cout << "Receive response list app successfull" << "\n";
    } 

    res.print();
}

void Client::StartApp(std::string name) {
    char* request = ConvertStringToChar("Start App");
    char* cName = new char[name.size() + 1];
    strcpy(cName, name.c_str());

    Request req(request, strlen(request), cName, strlen(cName));
    if (SendRequest(req, ServerSocket) == 0) {
        std::cerr << "Send request to server failed with error: " << WSAGetLastError() << "\n";
        exit(1);
    } else {
        std::cout << "Send request to server successfully" << "\n";
    } 

    // Receive data from server
    char recvbuf[DEFAULT_BUFLEN];
    Response res(recvbuf, DEFAULT_BUFLEN);
    if (RecvResponse(res, ServerSocket) == 0) {
        std::cerr << "Receive response start app failed with error: " << WSAGetLastError() << "\n";
        exit(1);
    } else {
        std::cout << "Receive response start app successfull" << "\n";
    } 

    res.print();
}

void Client::StopApp(std::string name) {
    char* request = ConvertStringToChar("Stop App");
    char* cName = new char [name.size() + 1];
    strcpy(cName, name.c_str());

    Request req(request, strlen(request), cName, strlen(cName));
    if (SendRequest(req, ServerSocket) == 0) {
        std::cerr << "Send request to server failed with error: " << WSAGetLastError() << "\n";
        exit(1);
    } else {
        std::cout << "Send request to server successfully" << "\n";
    } 

    // Receive data from server
    char recvbuf[DEFAULT_BUFLEN];
    Response res(recvbuf, DEFAULT_BUFLEN);
    if (RecvResponse(res, ServerSocket) == 0) {
        std::cerr << "Receive response stop app failed with error: " << WSAGetLastError() << "\n";
        exit(1);
    } else {
        std::cout << "Receive response stop app successfull" << "\n";
    } 
    
    res.print();
}

void Client::ListProcess() {
    char* request = ConvertStringToChar("List Process");
    char* nulString = ConvertStringToChar("\0");
    Request req(request, strlen(request), nulString, 0);
    if (SendRequest(req, ServerSocket) == 0) {
        std::cerr << "Send request to server failed with error: " << WSAGetLastError() << "\n";
        exit(1);
    } else {
        std::cout << "Send request to server successfully" << "\n";
    } 

    // Receive data from server
    char recvbuf[DEFAULT_BUFLEN];
    Response res(recvbuf, DEFAULT_BUFLEN);
    if (RecvResponse(res, ServerSocket) == 0) {
        std::cerr << "Receive response list app failed with error: " << WSAGetLastError() << "\n";
        // exit(1);
    } else {
        std::cout << "Receive response list app successfull" << "\n";
    } 
    
    res.print();
}

void Client::ListDirectory(std::string path) {
    char* request = ConvertStringToChar("List Directory");
    char* cPath = new char[path.size() + 1];
    strcpy(cPath, path.c_str());

    Request req(request, strlen(request), cPath, strlen(cPath));
    if (SendRequest(req, ServerSocket) == 0) {
        std::cerr << "Send request to server failed with error: " << WSAGetLastError() << "\n";
        exit(1);
    } else {
        std::cout << "Send request to server successfully" << "\n";
    } 

    // Receive data from server
    char recvbuf[DEFAULT_BUFLEN];
    Response res(recvbuf, DEFAULT_BUFLEN);
    // int iResult = recv(ServerSocket, recvbuf, DEFAULT_BUFLEN, 0);
    // std::cout << iResult << "\n";
    if (RecvResponse(res, ServerSocket) == false) {
        std::cerr << "Receive response list app failed with error: " << WSAGetLastError() << "\n";
        exit(1);
    } else {
        std::cout << "Receive response list app successfull" << "\n";
    } 
    
    res.print();
}

void Client::KeyLog() {
    char* request = ConvertStringToChar("Key Log");
    char* nulString = ConvertStringToChar("\0");
    Request req(request, strlen(request), nulString, 0);
    if (SendRequest(req, ServerSocket) == 0) {
        std::cerr << "Send request to server failed with error: " << WSAGetLastError() << "\n";
        exit(1);
    } else {
        std::cout << "Send request to server successfully" << "\n";
    } 

    // Receive data from server
    char recvbuf[DEFAULT_BUFLEN];
    do {
        int iResult = recv(ServerSocket, recvbuf, DEFAULT_BUFLEN, 0);
        if (iResult <= 0) {
            break;
        }

        recvbuf[iResult] = '\0'; 
        std::cout << recvbuf;
        if (strcmp(recvbuf, "Escape") == 0) {
            break;
        }
        
    } while (1);

    std::cout << "\nStop key log\n";
}

void Client::CaptureScreen() {
    char* request = ConvertStringToChar("Capture Screen");
    char* nulString = ConvertStringToChar("\0");
    Request req(request, strlen(request), nulString, 0);
    if (SendRequest(req, ServerSocket) == 0) {
        std::cerr << "Send request to server failed with error: " << WSAGetLastError() << "\n";
        exit(1);
    } else {
        std::cout << "Send request to server successfully" << "\n";
    } 


    FILE *picture = fopen(SaveScreenShotClient, "wb");

    std::string receiveData;     
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    int bytesRead = 0;
    do {
        bytesRead = recv(ServerSocket, recvbuf, recvbuflen, 0);
        if (bytesRead > 0) {
            fwrite(recvbuf, bytesRead, 1, picture);
        } else if (bytesRead <= 0) {
            break;
        } 
    } while (1);

    // write to file 
    fwrite(receiveData.c_str(), receiveData.length(), 1, picture);
    
    fclose(picture);

    // open picture
    ShellExecute(NULL, "open", SaveScreenShotClient, NULL, NULL, SW_SHOWDEFAULT);


    int iResult = shutdown(ServerSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "Shutdown failed with error: " << WSAGetLastError() << "\n";
    }
}