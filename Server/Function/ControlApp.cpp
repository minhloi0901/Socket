#include "ControlApp.h"

#include "../../Library.h"

const std::string REGISTRY_UNINSTALL_PATH = "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall";
const std::string REGISTRY_UNINSTALL_PATH_WOW = "Software\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall";

struct AppInfo {
    std::string name;
    std::string version;
    std::string date;
    std::string installLocation;
    std::string uninstallString;
    int regType;

    AppInfo(std::string _name, std::string _version, std::string _date, std::string _installLocation, std::string _uninstallString, int _regType) : 
    name(_name), version(_version), date(_date), installLocation(_installLocation), uninstallString(_uninstallString), regType(_regType) {}

    static bool cmp (const AppInfo& A, const AppInfo& B) {
        return (A.name < B.name);
    }
};


std::vector<AppInfo> getInstalledApp(int RegType) {
    std::vector<AppInfo> appNames;

    HKEY hKey;
    LONG res;
    if (RegType == 1) {
        // get installed app from HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall
        res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGISTRY_UNINSTALL_PATH.c_str(), 0, KEY_READ, &hKey);
    } else if (RegType == 2) {
        // get installed app from HKEY_LOCAL_MACHINE\\Software\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall
        res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGISTRY_UNINSTALL_PATH_WOW.c_str(), 0, KEY_READ, &hKey);
    } else {
        // get installed app from // HKEY_CURENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall
        res = RegOpenKeyEx(HKEY_CURRENT_USER, REGISTRY_UNINSTALL_PATH.c_str(), 0, KEY_READ, &hKey);
    }

    if (res != ERROR_SUCCESS) {
        std::cerr << "Error: could not open registry key " << REGISTRY_UNINSTALL_PATH << "\n";
        return appNames;
    }

    DWORD numSubKeys = 0;
    RegQueryInfoKey(hKey, NULL, NULL, NULL, &numSubKeys, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    for (DWORD i = 0; i < numSubKeys; i++) {
        DWORD bufSize = DEFAULT_BUFLEN;
        char buf[DEFAULT_BUFLEN];

        RegEnumKeyEx(hKey, i, buf, &bufSize, NULL, NULL, NULL, NULL);
        HKEY subKey;

        std::string subKeyPath;
        if (RegType == 2) {
            subKeyPath = REGISTRY_UNINSTALL_PATH_WOW + "\\" + buf;
        } else {
            subKeyPath = REGISTRY_UNINSTALL_PATH + "\\" + buf;
        }
        if (RegType == 3) {
            res = RegOpenKeyEx(HKEY_CURRENT_USER, subKeyPath.c_str(), 0, KEY_READ, &subKey);
        } else {
            res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, subKeyPath.c_str(), 0, KEY_READ, &subKey);
        }

        if (res != ERROR_SUCCESS) {
            continue;
        }
        // get system component first to check if its belong to installed app
        DWORD isSystemComponent = 0;
        DWORD dataSize = sizeof(isSystemComponent);
        res = RegQueryValueEx(subKey, "SystemComponent", NULL, NULL, (LPBYTE)&isSystemComponent, &dataSize);
        if (res == ERROR_SUCCESS && isSystemComponent == 1) {
            RegCloseKey(subKey);
            continue;
        }
        // get app name
        DWORD nameSize = DEFAULT_BUFLEN;
        res = RegQueryValueEx(subKey, "DisplayName", NULL, NULL, (LPBYTE)buf, &nameSize);
       
        if (res != ERROR_SUCCESS) {
            RegCloseKey(subKey);
            continue;
        }
        std::string appName = buf;
        // cout << appName << "\n";

        // get app version
        DWORD versionSize = DEFAULT_BUFLEN;
        res = RegQueryValueEx(subKey, "DisplayVersion", NULL, NULL, (LPBYTE)buf, &versionSize);
        std::string appVersion = "";
        if (res == ERROR_SUCCESS) {
            appVersion = buf;
        }

        // get app installlocation
        DWORD installLocationSize = DEFAULT_BUFLEN;
        res = RegQueryValueEx(subKey, "InstallLocation", NULL, NULL, (LPBYTE)buf, &installLocationSize);
        std::string appInstallLocation = "";
        if (res == ERROR_SUCCESS) {
            appInstallLocation = buf;
        }

        // get app install date
        DWORD installDateSize = DEFAULT_BUFLEN;
        res = RegQueryValueEx(subKey, "InstallDate", NULL, NULL, (LPBYTE)buf, &installDateSize);
        std::string appInstallDate = "";
        if (res == ERROR_SUCCESS) {
            appInstallDate = buf;
        }

        // get app uninstall string
        DWORD uninstallString = DEFAULT_BUFLEN;
        res = RegQueryValueEx(subKey, "UninstallString", NULL, NULL, (LPBYTE)buf, &uninstallString);
        std::string appUninstallString = "";
        if (res == ERROR_SUCCESS) {
            appUninstallString = buf;
        }

                
        if (!appName.empty()) {
            appNames.push_back(AppInfo(appName, appVersion, appInstallDate, appInstallLocation, appUninstallString, RegType));
        }

        RegCloseKey(subKey);
    }
    return appNames;
}

const std::string REGISTRY_UNINSTALL_START_PATH = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
const std::string REGISTRY_UNINSTALL_START_PATH_WOW = "Software\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Run";

bool startApplication(std::string name) {
    int AppPos;
    std::vector <AppInfo> ListInstalledApp;
    int pos = 0;
    for (int RegType = 1; RegType <= 3; RegType++) {
        for (const AppInfo& appInfo : getInstalledApp(RegType)) {  
            ListInstalledApp.push_back(appInfo);
            if (appInfo.name.compare(name) == 0) {
                AppPos = pos; 
            }
            pos++;
        }
    }

    const std::string& startApp = ListInstalledApp[AppPos].name;
    int RegType = ListInstalledApp[AppPos].regType;

    if (!ListInstalledApp[AppPos].installLocation.empty()) {
        std::string exePath = ListInstalledApp[AppPos].installLocation + "\\" + ListInstalledApp[AppPos].name + ".exe";
        STARTUPINFO si = {sizeof(si)};
        PROCESS_INFORMATION pi;
        if (CreateProcess(NULL, (LPSTR)exePath.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            std::cout << "App started successfully in case 1" << "\n";
            return 1;
            // continue;
        } else {
            std::cerr << "Failed to start app. Error code: " << GetLastError() << "\n";
        }
    }
    

    HKEY hKey;
    LONG res;
    if (RegType == 1) {
        res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGISTRY_UNINSTALL_START_PATH.c_str(), 0, KEY_READ, &hKey);
    } else if (RegType == 2) {
        res = RegOpenKeyEx(HKEY_LOCAL_MACHINE, REGISTRY_UNINSTALL_START_PATH_WOW.c_str(), 0, KEY_READ, &hKey);
    } else {
        res = RegOpenKeyEx(HKEY_CURRENT_USER, REGISTRY_UNINSTALL_START_PATH.c_str(), 0, KEY_READ, &hKey);
    }
  
    if (res != ERROR_SUCCESS) {
        std::cerr << "Failed to open registry key. Error code: " << res << "\n";
        return false;
    }

    DWORD index = 0;
    char valueName[DEFAULT_BUFLEN];
    DWORD valueNameSize = DEFAULT_BUFLEN;
    BYTE data[DEFAULT_BUFLEN];
    DWORD dataSize = DEFAULT_BUFLEN;
    DWORD type;
    
    while (RegEnumValue(hKey, index, valueName, &valueNameSize, NULL, &type, data, &dataSize) == ERROR_SUCCESS) {
        if (type == REG_SZ) {
            std::string name(valueName);
            if (name == startApp) { 
                std::string path((char*)data, dataSize);
                STARTUPINFO si = {sizeof(si)};
                PROCESS_INFORMATION pi;
                if (CreateProcess(NULL, (LPSTR)path.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
                    std::cout << "App started successfully in case 2" << "\n";
                    RegCloseKey(hKey);
                    return true;

                } else {
                    std::cerr << "Failed to start app. Error code: " << GetLastError() << "\n";
                    RegCloseKey(hKey);
                    return false;
                }
                break;
            }
        }

        valueNameSize = MAX_PATH;
        dataSize = MAX_PATH;
        index++;
    }
    
    RegCloseKey(hKey);
    
    return false;
}

std::vector<std::string> ListInstalledApp() {
    std::vector <std::string> allApp;
    for (int RegType = 1; RegType <= 3; RegType++) {
        for (const AppInfo& appInfo : getInstalledApp(RegType)) {
            allApp.push_back(appInfo.name);
        }
    }

    return allApp;
}

