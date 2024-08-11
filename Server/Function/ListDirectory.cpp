#include "ListDirectory.h"

#include "../../Library.h"

WIN32_FIND_DATAA fd;

std::string listDir(std::string path, int tab_cnt) {
    std::stringstream ss;
    // std::cout << path << "\n";
    std::string curPath = path + "\\*";
    WIN32_FIND_DATAA fileData;
    HANDLE hFind = FindFirstFile(curPath.c_str(), &fileData);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (strcmp(fd.cFileName, " ") == 0 || strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0) {
                continue;
            }
            // std::cout << "cout";
            for (int i = 0; i <= tab_cnt; i++) {
                ss << "  ";
                std::cout << "   ";
            }
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                ss << "." << fd.cFileName << "\n";
                // std::cout << ". " << fd.cFileName << "\n";
                ss << listDir(path + "\\" + fd.cFileName, tab_cnt + 1);
            } else {
                ss << "." << fd.cFileName << "\n";
                // std::cout << ". " << fd.cFileName << "\n";
            }
        } while (FindNextFileA(hFind, &fd));

        FindClose(hFind);
    }

    std::string dir = ss.str();
    return dir;
}
