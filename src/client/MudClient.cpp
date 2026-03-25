#include "client/MudClient.h"
#include "network/Network.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

namespace mud {

MudClient::MudClient(const std::string& host, short port)
    : host_(host), port_(port) {
}

MudClient::~MudClient() {
    disconnect();
    network::cleanup();
}

bool MudClient::connect() {
    if (!network::init()) {
        std::cerr << "网络初始化失败" << std::endl;
        return false;
    }
    
    socket_fd_ = network::connect_to_server(host_, port_);
    if (socket_fd_ == -1) {
        return false;
    }
    
    connected_ = true;
    return true;
}

void MudClient::disconnect() {
    running_ = false;
    connected_ = false;
    
    if (socket_fd_ != -1) {
        network::close_socket(socket_fd_);
        socket_fd_ = -1;
    }
}

void MudClient::send(const std::string& message) {
    if (!connected_ || socket_fd_ == -1) {
        return;
    }
    
    network::send_data(socket_fd_, message.c_str(), message.size());
}

void MudClient::start() {
    if (!connected_) {
        if (!connect()) {
            return;
        }
    }
    
    std::cout << "\n=== MUD 客户端已启动 ===" << std::endl;
    std::cout << "输入命令与服务器交互，输入 'quit' 或 'exit' 退出客户端" << std::endl;
    std::cout << "========================\n" << std::endl;
    
    running_ = true;
    
    // 启动读取线程
    std::thread(&MudClient::readLoop, this).detach();
    
    // 主线程读取用户输入
    std::string line;
    while (running_ && std::getline(std::cin, line)) {
        if (line == "quit" || line == "exit") {
            break;
        }
        
        if (!line.empty()) {
            line += "\n";
            send(line);
        }
    }
    
    disconnect();
}

void MudClient::readLoop() {
    static constexpr size_t buffer_size = 4096;
    char buffer[buffer_size];
    
    while (running_ && connected_) {
        ssize_t bytes_read = network::recv_data(socket_fd_, buffer, buffer_size);
        
        if (bytes_read <= 0) {
            if (bytes_read < 0 && running_) {
                std::cerr << "连接已关闭" << std::endl;
            }
            break;
        }
        
        // 输出到控制台
        std::cout.write(buffer, bytes_read);
        std::cout.flush();
    }
}

} // namespace mud
