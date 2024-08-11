#include "ListProcess.h"

#include "../../Library.h"


std::vector <ProcessInfo> ProcessList() {
    DWORD processes[DEFAULT_BUFLEN], cbNeeded, cProcesses;

    if (!EnumProcesses(processes, sizeof(processes), &cbNeeded)) {
        std::cerr << "Cannot list" << "\n";
        return {};
    }

    cProcesses = cbNeeded / sizeof(DWORD);

    std::vector <ProcessInfo> ProcessList;
    for (int i = 0; i < cProcesses; i++) {
        if (processes[i]) {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processes[i]);
            if (hProcess != NULL) {
                HMODULE hMod;
                DWORD cbNeeded;
                if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded)) {
                    char szProcessName[MAX_PATH] = {0};
                    if (GetModuleBaseNameA(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(char))) {
                        ProcessList.push_back(ProcessInfo(szProcessName, processes[i]));
                    }
                }
                CloseHandle(hProcess);
            }
        }
    }

    return ProcessList;
}

std::vector <std::string> ListProcessApp() {
    std::vector <ProcessInfo> list = ProcessList();
    std::vector <std::string> allProcess;
    for (const ProcessInfo& ProcessInfo : list) {
        allProcess.push_back(ProcessInfo.name);
    }

    return allProcess;
}

bool stopProcessApp(std::string name) {
    DWORD pID;
    bool check = 0; 
    for (const ProcessInfo& ProcessInfo : ProcessList()) {
        if (ProcessInfo.name == name) {
            pID = ProcessInfo.ID;
            check = 1;
            break;
        }    
    }

    if (!check) {
        return false;
    }

    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pID);
    if (hProcess == NULL) {
        std::cerr << "cannot stop running" << "\n";
        return false;
    }

    bool result = TerminateProcess(hProcess, 0);
    if (!result) {
        std::cerr << "cannot stop running" << "\n";
        return false;
    }

    CloseHandle(hProcess);
    return true;
}

