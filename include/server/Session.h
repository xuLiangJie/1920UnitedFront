#ifndef SESSION_H
#define SESSION_H

#include <string>
#include <memory>
#include <queue>
#include <mutex>
#include <atomic>
#include "player/Player.h"
#include "command/CommandHandler.h"
#include "world/Room.h"
#include "combat/Combat.h"

namespace mud {

class MudServer;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(int socket_fd, uint32_t session_id, MudServer& server);
    ~Session();
    
    void start();
    void stop();
    
    uint32_t getId() const { return session_id_; }
    Player& getPlayer() { return player_; }
    const Player& getPlayer() const { return player_; }
    
    void send(const std::string& message);
    
    int getSocketFd() const { return socket_fd_; }
    MudServer& getServer() { return server_; }
    const MudServer& getServer() const { return server_; }
    
    Room* getCurrentRoom();
    const Room* getCurrentRoom() const;
    void setCurrentRoom(const std::string& room_id);
    
    // 战斗相关
    CombatManager& getCombatManager() { return combat_manager_; }
    const CombatManager& getCombatManager() const { return combat_manager_; }
    bool isInCombat() const { return combat_manager_.isInCombat(); }
    
private:
    void readLoop();
    void writeLoop();
    
    int socket_fd_;
    uint32_t session_id_;
    MudServer& server_;
    Player player_;
    CommandHandler command_handler_;
    CombatManager combat_manager_;
    std::string current_room_id_;
    
    std::queue<std::string> write_queue_;
    std::mutex write_mutex_;
    std::atomic<bool> stopped_{false};
};

} // namespace mud

#endif // SESSION_H
