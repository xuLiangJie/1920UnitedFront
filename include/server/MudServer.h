#ifndef MUD_SERVER_H
#define MUD_SERVER_H

#include <string>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <vector>
#include <atomic>
#include "world/Room.h"

namespace mud {

class Session;

class MudServer {
public:
    MudServer(short port);
    ~MudServer();
    
    void start();
    void stop();
    
    void addSession(std::shared_ptr<Session> session);
    void removeSession(uint32_t session_id);
    
    std::vector<std::string> getOnlinePlayers() const;
    std::vector<std::string> getPlayersInRoom(const std::string& room_id, uint32_t exclude_id = 0) const;
    
    void broadcast(const std::string& message, uint32_t exclude_id = 0);
    void broadcastToRoom(const std::string& room_id, const std::string& message, uint32_t exclude_id = 0);
    
    short getPort() const { return port_; }
    World& getWorld() { return world_; }
    const World& getWorld() const { return world_; }
    
private:
    short port_;
    std::atomic<bool> running_{false};
    
    std::unordered_map<uint32_t, std::shared_ptr<Session>> sessions_;
    mutable std::mutex sessions_mutex_;
    uint32_t next_session_id_ = 1;
    
    World world_;
    int server_socket_ = -1;
};

} // namespace mud

#endif // MUD_SERVER_H
