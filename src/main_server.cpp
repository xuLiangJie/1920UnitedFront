#include <iostream>
#include <csignal>
#include <atomic>
#include "server/MudServer.h"
#include "network/Network.h"

namespace {
    std::atomic<bool> g_shutdown{false};
    mud::MudServer* g_server = nullptr;
    
    void signal_handler(int) {
        g_shutdown = true;
        if (g_server) {
            g_server->stop();
        }
    }
}

int main(int argc, char* argv[]) {
    try {
        short port = 8888;
        
        if (argc > 1) {
            port = static_cast<short>(std::stoi(argv[1]));
        }
        
        std::signal(SIGINT, signal_handler);
        std::signal(SIGTERM, signal_handler);
        
        std::cout << "+------------------------------------------+" << std::endl;
        std::cout << "|      Western Fantasy MUD Game Server     |" << std::endl;
        std::cout << "+------------------------------------------+" << std::endl;
        std::cout << std::endl;
        
        mud::MudServer server(port);
        g_server = &server;
        
        server.start();
        
        g_server = nullptr;
        mud::network::cleanup();
        
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        mud::network::cleanup();
        return 1;
    }
    
    return 0;
}
