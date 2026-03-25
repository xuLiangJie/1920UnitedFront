#ifndef MUD_CLIENT_H
#define MUD_CLIENT_H

#include <string>
#include <atomic>

namespace mud {

class MudClient {
public:
    MudClient(const std::string& host, short port);
    ~MudClient();
    
    bool connect();
    void start();
    void disconnect();
    
    void send(const std::string& message);
    
private:
    void readLoop();
    
    int socket_fd_ = -1;
    std::string host_;
    short port_;
    std::atomic<bool> connected_{false};
    std::atomic<bool> running_{false};
};

} // namespace mud

#endif // MUD_CLIENT_H
