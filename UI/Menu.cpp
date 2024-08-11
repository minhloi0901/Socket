#include "../Library.h"
#include "../Client/Client.h"

#include "Menu.h"

void showMenu(Client& c) {
    std::vector <std::string> command {
        "Exit",
        "List App",
        "Start App",
        "Stop App",
        "List Process",
        "List Directory",
        "Key Log",
        "Capture Screen",
    };

    do {
        std::cout << "Please select one option!" << "\n";
        for (int i = 0; i < command.size(); i++) {
            std::cout << i  << " " << command[i] << "\n";
        }

        std::cout << "Select (0 - " << command.size() - 1 << ")" << "\n";
        std::string choice;
        std::cout << "Option: ";
        std::getline(std::cin, choice);

        int choice_num = std::stoi(choice);
        if (choice_num < 0 || choice_num > command.size()) {
            std::cout << "Invalid option!" << "\n";
        } else if (choice_num == 1) { 
            c.ListApp();
        } else if (choice_num == 2) {
            std::cout << "Name of app that want to start: ";
            std::string name;
            std::getline(std::cin, name);
            c.StartApp(name);
        } else if (choice_num == 3) {
            std::cout << "Name of app that want to stop: ";
            std::string name;
            std::getline(std::cin, name);
            c.StopApp(name);
        } else if (choice_num == 4) {
            c.ListProcess();
        } else if (choice_num == 5) {
            std::cout << "Path of directory that want to list: ";
            std::string path;
            std::getline(std::cin, path);
            c.ListDirectory(path);
        } else if (choice_num == 6) {
            c.KeyLog();
        } else if (choice_num == 7) {
            c.CaptureScreen();
        } else if (choice_num == 0) {
            std::cout << "Turn off client" << "\n";
            break;
        }

        system("pause");

        std::cout << "Continue?" << "\n";
        std::cout << "Option(Y/N): ";
        std::getline(std::cin, choice);
        if (choice == "Y") {
            continue;
        } else if (choice == "N") {
            std::cout << "Exit Socket" << "\n";
            break;
        } else {
            std::cerr << "Error syntax" << "\n";
            break;
        }
        break;
        
    } while (1);

    return;
}