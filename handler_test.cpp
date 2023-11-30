#include "system_command_handler.h"
#include <iostream>

std::string displayMainMenu() {
    std::string choice;
    std::cout << "Select between: volume, media, shutdown, restart, sleep, or exit" << std::endl;
    std::cout << "Enter your choice: ";
    std::cin >> choice;
    std::cin.clear();
    std::cin.ignore(9999, '\n');
    return choice;
}

void volumeTest(VolumeControl& volumeControl) {
    std::string input;
    std::cout << "Enter volume levels (0-100) to adjust the system volume. Type 'exit' to quit." << std::endl;

    while (true) {
        std::cout << "Set volume to: ";
        std::getline(std::cin, input);

        if (input == "exit") {
            break;
        }

        try {
            int volumeLevel = std::stoi(input);
            if (volumeLevel >= 0 && volumeLevel <= 100) {
                if (volumeControl.handleCommand(input)) {
                    std::cout << "Volume set to " << input << std::endl;
                }
                else {
                    std::cout << "handler Error" << std::endl;
                }
            }
            else {
                std::cout << "Failed to set volume. Please enter a number between 0 and 100" << std::endl;
            }
        }

        catch (const std::invalid_argument&) {
            std::cout << "Failed to set volume. Please enter a number between 0 and 100" << std::endl;
        }
    }
}

void mediaTest(MediaControl& mediaControl) {
    std::cout << "play, pause, next, or prev" << std::endl;
    while (true) {
        std::string input;
        std::getline(std::cin, input);

        if (input == "play" || input == "pause" || input == "next" || input == "prev") {
            mediaControl.handleCommand(input);
        }
        else if (input == "exit") {
            break;
        }
        else {
            std::cout << "Failed to execute command. Select between: play, pause, next, or prev" << std::endl;
        }
    }
}

void shutdownTest(ShutdownProtocol& shutdownProtocol) {
    std::cout << "Are you sure you want to shutdown your device (y/n)?" << std::endl;
    std::string input;
    std::getline(std::cin, input);
    if (input == "y" || input == "Y") {
        shutdownProtocol.handleCommand("shutdown");
    }
    else {
        return;
    }
}

void sleepTest(SleepProtocol& sleepProtocol) {
    std::cout << "Are you sure you want to set your device to sleep (y/n)?" << std::endl;
    std::string input;
    std::getline(std::cin, input);
    if (input == "y" || input == "Y") {
        sleepProtocol.handleCommand("sleep");
    }
    else {
        return;
    }
}

void restartTest(RestartProtocol& restartProtocol) {
    std::cout << "Are you sure you want to restart your device (y/n)?" << std::endl;
    std::string input;
    std::getline(std::cin, input);
    if (input == "y" || input == "Y") {
        restartProtocol.handleCommand("restart");
    }
    else {
        return;
    }
}


int main() {
    VolumeControl volumeControl;
    MediaControl mediaControl;
    ShutdownProtocol shutdownProtocol;
    SleepProtocol sleepProtocol;
    RestartProtocol restartProtocol;

    while (true) {
        std::string command = displayMainMenu();

        if (command == "exit") {
            break;
        }
        if (command == "volume") {
            volumeTest(volumeControl);
        }

        if (command == "media") {
            mediaTest(mediaControl);
        }

        if (command == "shutdown") {
            shutdownTest(shutdownProtocol);
        }

        if (command == "sleep") {
            sleepTest(sleepProtocol);
        }

        if (command == "restart") {
            restartTest(restartProtocol);
        }
    }

    return 0;
}