#ifndef EVENTS_H
#define EVENTS_H

#include <string>
#include <memory>
#include <functional>
#include <any>
#include <unordered_map>
#include <cstdint>

namespace mud {

// 事件类型枚举
enum class EventType {
    // 战斗事件
    CombatStarted,
    CombatEnded,
    PlayerAttacked,
    MonsterAttacked,
    PlayerDied,
    MonsterDied,
    
    // 玩家事件
    PlayerJoined,
    PlayerLeft,
    PlayerMoved,
    PlayerNameChanged,
    
    // 消息事件
    MessageSent,
    BroadcastMessage,
    
    // 存档事件
    GameSaved,
    GameLoaded,
    
    // 自定义事件
    Custom
};

// 事件基类
class Event {
public:
    virtual ~Event() = default;
    virtual EventType getType() const = 0;
};

// 战斗开始事件
class CombatStartedEvent : public Event {
public:
    uint32_t sessionId;
    std::string monsterName;
    std::string monsterDescription;
    
    CombatStartedEvent(uint32_t sid, const std::string& mname, const std::string& mdesc)
        : sessionId(sid), monsterName(mname), monsterDescription(mdesc) {}
    
    EventType getType() const override { return EventType::CombatStarted; }
};

// 战斗结束事件
class CombatEndedEvent : public Event {
public:
    uint32_t sessionId;
    bool playerWon;
    int expGained;
    std::string monsterName;
    
    CombatEndedEvent(uint32_t sid, bool won, int exp, const std::string& mname)
        : sessionId(sid), playerWon(won), expGained(exp), monsterName(mname) {}
    
    EventType getType() const override { return EventType::CombatEnded; }
};

// 玩家攻击事件
class PlayerAttackedEvent : public Event {
public:
    uint32_t sessionId;
    int damage;
    bool isCrit;
    std::string monsterName;
    
    PlayerAttackedEvent(uint32_t sid, int dmg, bool crit, const std::string& mname)
        : sessionId(sid), damage(dmg), isCrit(crit), monsterName(mname) {}
    
    EventType getType() const override { return EventType::PlayerAttacked; }
};

// 怪物攻击事件
class MonsterAttackedEvent : public Event {
public:
    uint32_t sessionId;
    int damage;
    std::string monsterName;
    
    MonsterAttackedEvent(uint32_t sid, int dmg, const std::string& mname)
        : sessionId(sid), damage(dmg), monsterName(mname) {}
    
    EventType getType() const override { return EventType::MonsterAttacked; }
};

// 玩家死亡事件
class PlayerDiedEvent : public Event {
public:
    uint32_t sessionId;
    
    PlayerDiedEvent(uint32_t sid)
        : sessionId(sid) {}
    
    EventType getType() const override { return EventType::PlayerDied; }
};

// 玩家移动事件
class PlayerMovedEvent : public Event {
public:
    uint32_t sessionId;
    std::string playerName;
    std::string fromRoom;
    std::string toRoom;
    std::string direction;
    
    PlayerMovedEvent(uint32_t sid, const std::string& pname, 
                     const std::string& from, const std::string& to,
                     const std::string& dir)
        : sessionId(sid), playerName(pname), fromRoom(from), toRoom(to), direction(dir) {}
    
    EventType getType() const override { return EventType::PlayerMoved; }
};

// 玩家加入事件
class PlayerJoinedEvent : public Event {
public:
    uint32_t sessionId;
    std::string playerName;
    std::string roomId;
    
    PlayerJoinedEvent(uint32_t sid, const std::string& pname, const std::string& rid)
        : sessionId(sid), playerName(pname), roomId(rid) {}
    
    EventType getType() const override { return EventType::PlayerJoined; }
};

// 玩家离开事件
class PlayerLeftEvent : public Event {
public:
    uint32_t sessionId;
    std::string playerName;
    std::string roomId;
    
    PlayerLeftEvent(uint32_t sid, const std::string& pname, const std::string& rid)
        : sessionId(sid), playerName(pname), roomId(rid) {}
    
    EventType getType() const override { return EventType::PlayerLeft; }
};

// 广播消息事件
class BroadcastMessageEvent : public Event {
public:
    std::string roomId;
    std::string message;
    uint32_t excludeId;
    
    BroadcastMessageEvent(const std::string& rid, const std::string& msg, uint32_t exclude = 0)
        : roomId(rid), message(msg), excludeId(exclude) {}
    
    EventType getType() const override { return EventType::BroadcastMessage; }
};

// 通用消息事件(发送给特定会话)
class MessageSentEvent : public Event {
public:
    uint32_t sessionId;
    std::string message;
    
    MessageSentEvent(uint32_t sid, const std::string& msg)
        : sessionId(sid), message(msg) {}
    
    EventType getType() const override { return EventType::MessageSent; }
};

} // namespace mud

#endif // EVENTS_H
