#include "server/MudServer.h"
#include "server/Session.h"
#include "player/Player.h"
#include "network/Network.h"
#include <iostream>
#include <thread>
#include <chrono>

namespace mud {

MudServer::MudServer(short port) : port_(port) {
}

MudServer::~MudServer() {
    stop();
    network::cleanup();
}

void MudServer::start() {
    if (!network::init()) {
        std::cerr << "Network initialization failed" << std::endl;
        return;
    }
    
    server_socket_ = network::create_tcp_socket();
    if (server_socket_ == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return;
    }
    
    if (!network::bind_socket(server_socket_, port_)) {
        network::close_socket(server_socket_);
        return;
    }
    
    if (!network::listen_socket(server_socket_)) {
        network::close_socket(server_socket_);
        return;
    }
    
    std::cout << "+------------------------------------------+" << std::endl;
    std::cout << "|      Western Fantasy MUD Game Server     |" << std::endl;
    std::cout << "+------------------------------------------+" << std::endl;
    std::cout << std::endl;
    std::cout << "Server started, listening on port: " << port_ << std::endl;
    std::cout << "World loaded with " << world_.getRoom("hall")->getName() << " as starting point." << std::endl;
    std::cout << "Press Ctrl+C to stop" << std::endl;
    std::cout << std::endl;
    
    running_ = true;
    
    while (running_) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(server_socket_, &read_fds);
        
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        
        int result = select(static_cast<int>(server_socket_) + 1, &read_fds, nullptr, nullptr, &timeout);
        
        if (result > 0 && FD_ISSET(server_socket_, &read_fds)) {
            int client_fd = network::accept_socket(server_socket_);
            if (client_fd != -1) {
                std::cout << "New player connected from socket " << client_fd << std::endl;
                
                auto session = std::make_shared<Session>(
                    client_fd, next_session_id_++, *this);
                
                addSession(session);
                session->start();
            }
        } else if (result == -1) {
            if (running_) {
                std::cerr << "Accept error: " << network::get_last_error() << std::endl;
            }
        }
    }
}

void MudServer::stop() {
    running_ = false;
    
    {
        std::lock_guard<std::mutex> lock(sessions_mutex_);
        for (auto& [id, session] : sessions_) {
            session->stop();
        }
        sessions_.clear();
    }
    
    if (server_socket_ != -1) {
        network::close_socket(server_socket_);
        server_socket_ = -1;
    }
}

void MudServer::addSession(std::shared_ptr<Session> session) {
    std::lock_guard<std::mutex> lock(sessions_mutex_);
    sessions_[session->getId()] = session;
}

void MudServer::removeSession(uint32_t session_id) {
    std::lock_guard<std::mutex> lock(sessions_mutex_);
    
    auto it = sessions_.find(session_id);
    if (it != sessions_.end()) {
        std::cout << "Player disconnected: " << it->second->getPlayer().getName() << std::endl;
        sessions_.erase(it);
    }
}

std::vector<std::string> MudServer::getOnlinePlayers() const {
    std::lock_guard<std::mutex> lock(sessions_mutex_);
    
    std::vector<std::string> players;
    for (const auto& [id, session] : sessions_) {
        players.push_back(session->getPlayer().getName());
    }
    return players;
}

std::vector<std::string> MudServer::getPlayersInRoom(const std::string& room_id, uint32_t exclude_id) const {
    std::lock_guard<std::mutex> lock(sessions_mutex_);
    
    std::vector<std::string> players;
    for (const auto& [id, session] : sessions_) {
        if (id != exclude_id && session->getCurrentRoom() && 
            session->getCurrentRoom()->getId() == room_id) {
            players.push_back(session->getPlayer().getName());
        }
    }
    return players;
}

void MudServer::broadcast(const std::string& message, uint32_t exclude_id) {
    std::lock_guard<std::mutex> lock(sessions_mutex_);
    
    for (auto& [id, session] : sessions_) {
        if (id != exclude_id) {
            session->send(message);
        }
    }
}

void MudServer::broadcastToRoom(const std::string& room_id, const std::string& message, uint32_t exclude_id) {
    std::lock_guard<std::mutex> lock(sessions_mutex_);
    
    for (auto& [id, session] : sessions_) {
        if (id != exclude_id && session->getCurrentRoom() && 
            session->getCurrentRoom()->getId() == room_id) {
            session->send(message);
        }
    }
}

} // namespace mud
