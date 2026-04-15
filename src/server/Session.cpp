#include "server/Session.h"
#include "server/MudServer.h"
#include "player/Player.h"
#include "event/EventBus.h"
#include "network/Network.h"
#include <iostream>
#include <thread>
#include <sstream>

namespace mud {

Session::Session(int socket_fd, uint32_t session_id, MudServer& server)
    : socket_fd_(socket_fd)
    , session_id_(session_id)
    , server_(server)
    , player_()
    , current_room_id_("hall") {
}

Session::~Session() {
    stop();
}

void Session::start() {
    // 创建连接
    connection_ = std::make_unique<Connection>(socket_fd_);
    
    // 设置回调
    connection_->onMessage([this](const std::string& msg) {
        onMessage(msg);
    });
    
    connection_->onClose([this]() {
        onClose();
    });
    
    // 启动连接
    connection_->start();
    
    // 设置事件监听
    setupEventListeners();
    
    // 发送欢迎消息
    send("\r\n+-------------------------------------------------------+\r\n");
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
}

void Session::stop() {
    bool expected = false;
    if (!stopped_.compare_exchange_strong(expected, true)) {
        return;
    }
    
    if (connection_) {
        connection_->stop();
    }
    
    server_.removeSession(session_id_);
}

void Session::send(const std::string& message) {
    if (stopped_ || !connection_) {
        return;
    }
    
    connection_->send(message);
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

void Session::onMessage(const std::string& message) {
    if (stopped_ || message.empty()) {
        return;
    }
    
    // 使用命令注册中心执行命令
    std::string response = CommandRegistry::getInstance().executeCommand(this, message);
    if (!response.empty()) {
        send(response);
    }
}

void Session::onClose() {
    stop();
}

void Session::setupEventListeners() {
    uint32_t sessionId = session_id_;
    
    // 监听战斗开始事件
    EventBus::getInstance().subscribe(EventType::CombatStarted, [this, sessionId](const Event& e) {
        const auto& event = static_cast<const CombatStartedEvent&>(e);
        if (event.sessionId == sessionId) {
            std::ostringstream oss;
            oss << "\r\n====== COMBAT STARTED ======\r\n\r\n";
            oss << "A " << event.monsterName << " appears before you!\r\n";
            oss << event.monsterDescription << "\r\n\r\n";
            oss << "\r\nType 'attack' to fight!\r\n";
            oss << "============================\r\n";
            send(oss.str());
        }
    });
    
    // 监听战斗结束事件
    EventBus::getInstance().subscribe(EventType::CombatEnded, [this, sessionId](const Event& e) {
        const auto& event = static_cast<const CombatEndedEvent&>(e);
        if (event.sessionId == sessionId) {
            if (!event.playerWon) {
                // 玩家失败,传送回大厅
                setCurrentRoom("hall");
            }
        }
    });
    
    // 监听玩家死亡事件
    EventBus::getInstance().subscribe(EventType::PlayerDied, [this, sessionId](const Event& e) {
        const auto& event = static_cast<const PlayerDiedEvent&>(e);
        if (event.sessionId == sessionId) {
            std::ostringstream oss;
            oss << "\r\n*** DEFEAT ***\r\n";
            oss << "You black out and wake up back at the Starting Hall.\r\n";
            send(oss.str());
            
            // 恢复HP
            player_.setCurrentHP(player_.getMaxHP());
            player_.setCurrentMP(player_.getMaxMP());
        }
    });
    
    // 监听消息发送事件
    EventBus::getInstance().subscribe(EventType::MessageSent, [this, sessionId](const Event& e) {
        const auto& event = static_cast<const MessageSentEvent&>(e);
        if (event.sessionId == sessionId) {
            send(event.message);
        }
    });
}

} // namespace mud
