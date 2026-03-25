#include "server/Session.h"
#include "server/MudServer.h"
#include "player/Player.h"
#include "command/CommandHandler.h"
#include "network/Network.h"
#include <iostream>
#include <thread>
#include <chrono>

namespace mud {

Session::Session(int socket_fd, uint32_t session_id, MudServer& server)
    : socket_fd_(socket_fd)
    , session_id_(session_id)
    , server_(server)
    , player_()
    , command_handler_()
    , current_room_id_("hall") {
}

Session::~Session() {
    stop();
}

void Session::start() {
    send("\r\n+-------------------------------------------------------+\r\n");
    send("|                                                       |\r\n");
    send("|     Welcome to Western Fantasy MUD Game!              |\r\n");
    send("|                                                       |\r\n");
    send("+-------------------------------------------------------+\r\n");
    send("\r\n");
    send("You are an adventurer entering this mysterious world.\r\n");
    send("Explore unknown lands, meet other adventurers,\r\n");
    send("and experience a wonderful fantasy journey.\r\n");
    send("\r\n");
    send("Type 'help' to see available commands.\r\n");
    send("Type 'name <yourname>' to set your character name.\r\n");
    send("\r\n");

    std::thread(&Session::readLoop, shared_from_this()).detach();
    std::thread(&Session::writeLoop, shared_from_this()).detach();
}

void Session::stop() {
    bool expected = false;
    if (!stopped_.compare_exchange_strong(expected, true)) {
        return;
    }

    network::close_socket(socket_fd_);
    server_.removeSession(session_id_);
}

void Session::send(const std::string& message) {
    if (stopped_) {
        return;
    }

    std::lock_guard<std::mutex> lock(write_mutex_);
    write_queue_.push(message);
}

Room* Session::getCurrentRoom() {
    return server_.getWorld().getRoom(current_room_id_);
}

const Room* Session::getCurrentRoom() const {
    return server_.getWorld().getRoom(current_room_id_);
}

void Session::setCurrentRoom(const std::string& room_id) {
    current_room_id_ = room_id;
    player_.setLocation(room_id);
}

void Session::readLoop() {
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
                    std::string response = command_handler_.handleCommand(this, line_buffer);
                    if (!response.empty()) {
                        send(response);
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

void Session::writeLoop() {
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
