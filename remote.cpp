#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <windows.h>
#include <functional>
#include <iostream>
#include <sstream>
#include <memory>
#include <map>

#include "system_command_handler.h"

typedef websocketpp::server<websocketpp::config::asio> server;


std::map<std::string, std::shared_ptr<SystemCommandHandler>> commandHandlers;

void initCommandHandlers() {
    commandHandlers["VolumeControl"] = std::make_shared<VolumeControl>();
    commandHandlers["MediaControl"] = std::make_shared<MediaControl>();
    commandHandlers["Shutdown"] = std::make_shared<ShutdownProtocol>();
    commandHandlers["Sleep"] = std::make_shared<SleepProtocol>();
    commandHandlers["Restart"] = std::make_shared<RestartProtocol>();
}

void on_message(server* s, websocketpp::connection_hdl hdl, server::message_ptr msg) {
    std::string payload = msg->get_payload();
    std::cout << "Received message: " << payload << std::endl;

    std::istringstream iss(payload);
    std::string commandType;
    iss >> commandType;

    std::string commandParams;
    iss >> commandParams;
    
    std::cout << "Command Type: " << commandType << ", Params: " << commandParams << std::endl;

    auto send_message = [&](const std::string& response) {
        s->send(hdl, response, msg->get_opcode());
        };

    auto handlerIt = commandHandlers.find(commandType);
    if (handlerIt != commandHandlers.end()) {
        std::cout << "Executing command handler for: " << commandType << std::endl;
        if (handlerIt->second->handleCommand(commandParams)) {
            send_message(commandType + " command executed successfully.");
        }
        else {
            send_message("Failed to execute " + commandType + " command.");
        }
    }
    else {
        send_message("Unknown command type: " + commandType);
    }
}



int main() {
    // Initialize Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != NO_ERROR) {
        std::cerr << "WSAStartup failed with error: " << result << std::endl;
        return 1;
    }

    try {
        // Initialize command handlers
        initCommandHandlers();

        server endpoint;
        endpoint.init_asio();

        using std::placeholders::_1;
        using std::placeholders::_2;
        endpoint.set_message_handler(bind(&on_message, &endpoint, _1, _2));

        endpoint.listen(9002);
        endpoint.start_accept();
        endpoint.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    WSACleanup();
    return 0;
}