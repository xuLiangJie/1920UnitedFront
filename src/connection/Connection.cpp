#include "connection/Connection.h"
#include "network/Network.h"
#include <iostream>
#include <thread>
#include <chrono>

namespace mud {

Connection::Connection(int socket_fd)
    : socket_fd_(socket_fd) {
}

Connection::~Connection() {
    stop();
}

void Connection::start() {
    std::thread(&Connection::readLoop, this).detach();
    std::thread(&Connection::writeLoop, this).detach();
}

void Connection::stop() {
    bool expected = false;
    if (!stopped_.compare_exchange_strong(expected, true)) {
        return;
    }
    
    network::close_socket(socket_fd_);
    
    if (on_close_) {
        on_close_();
    }
}

void Connection::send(const std::string& message) {
    if (stopped_) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(write_mutex_);
    write_queue_.push(message);
}

void Connection::readLoop() {
    static constexpr size_t buffer_size = 1024;
    char buffer[buffer_size];
    std::string line_buffer;
    
    while (!stopped_) {
        ssize_t bytes_read = network::recv_data(socket_fd_, buffer, buffer_size);
        
        if (bytes_read <= 0) {
            break;
        }
        
        for (ssize_t i = 0; i < bytes_read; ++i) {
            char c = buffer[i];
            
            if (c == '\n' || c == '\r') {
                if (!line_buffer.empty()) {
                    if (on_message_) {
                        on_message_(line_buffer);
                    }
                    line_buffer.clear();
                }
            } else {
                line_buffer += c;
            }
        }
    }
    
    stop();
}

void Connection::writeLoop() {
    while (!stopped_) {
        std::string message;
        
        {
            std::lock_guard<std::mutex> lock(write_mutex_);
            if (write_queue_.empty()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }
            message = write_queue_.front();
            write_queue_.pop();
        }
        
        ssize_t bytes_sent = network::send_data(socket_fd_, message.c_str(), message.size());
        if (bytes_sent <= 0) {
            break;
        }
    }
}

} // namespace mud
