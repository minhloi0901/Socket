#pragma once

#include "../Library.h"

bool RecvAll(SOCKET s, char* buf, int &len);

bool SendAll(SOCKET s, char* buf, int &len);

char* ConvertStringToChar(std::string s);