#pragma once

#include <string>
#include <vector>
#include <winsock2.h>
#include <windows.h>

class ProcessInfo {
public:
    std::string name;
    DWORD ID;

public:
    ProcessInfo(std::string _name, DWORD _ID) : name(_name), ID(_ID) {}
};

std::vector <std::string> ListProcessApp();

bool stopProcessApp(std::string name);


