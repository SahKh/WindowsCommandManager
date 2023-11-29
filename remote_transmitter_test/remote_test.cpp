#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <iostream>
#include <functional>
#include <string>

typedef websocketpp::client<websocketpp::config::asio_client> client;

void command_input_thread(client* c, websocketpp::connection_hdl hdl) {
    while (true) {
        std::string command;
        std::getline(std::cin, command);

        if (command == "exit") {
            c->close(hdl, websocketpp::close::status::normal, "");
            break;
        }

        c->send(hdl, command, websocketpp::frame::opcode::text);
    }
}

void on_open(client* c, websocketpp::connection_hdl hdl) {
    std::cout << "Connected to server. Type a command (volume <level>, media <command>, shutdown, sleep, restart, exit to close):" << std::endl;

    // Start the command input thread
    std::thread input_thread(command_input_thread, c, hdl);
    input_thread.detach(); // Detach the thread to run independently
}


void on_message(websocketpp::connection_hdl, client::message_ptr msg) {
    std::cout << "Received reply: " << msg->get_payload() << std::endl;
}

int main() {
    client c;

    std::string uri = "ws://67.149.157.3:9002";

    try {
        // Set logging to be less verbose
        c.set_access_channels(websocketpp::log::alevel::all);
        c.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize ASIO
        c.init_asio();

        // Register our handlers
        c.set_message_handler(&on_message);
        c.set_open_handler(std::bind(&on_open, &c, std::placeholders::_1));

        websocketpp::lib::error_code ec;
        client::connection_ptr con = c.get_connection(uri, ec);
        if (ec) {
            std::cout << "could not create connection because: " << ec.message() << std::endl;
            return -1;
        }

        c.connect(con);

        // Start the ASIO io_service run loop in a separate thread
        std::thread asio_thread([&c]() { c.run(); });

        asio_thread.join();
    }
    catch (websocketpp::exception const& e) {
        std::cout << e.what() << std::endl;
    }
}

