#include <iostream>
#include "client/MudClient.h"
#include "network/Network.h"

int main(int argc, char* argv[]) {
    try {
        std::string host = "127.0.0.1";
        short port = 8888;
        
        if (argc > 1) {
            host = argv[1];
        }
        if (argc > 2) {
            port = static_cast<short>(std::stoi(argv[2]));
        }
        
        std::cout << "+------------------------------------------+" << std::endl;
        std::cout << "|      Western Fantasy MUD Game Client     |" << std::endl;
        std::cout << "+------------------------------------------+" << std::endl;
        std::cout << std::endl;
        
        mud::MudClient client(host, port);
        client.start();
        
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        mud::network::cleanup();
        return 1;
    }
    
    return 0;
}
