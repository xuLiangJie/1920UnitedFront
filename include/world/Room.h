#ifndef ROOM_H
#define ROOM_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "combat/Monster.h"

namespace mud {

enum class Direction {
    North,
    South,
    East,
    West,
    Up,
    Down,
    Enter,
    Exit,
    None
};

struct NPC {
    std::string name;
    std::string description;
    std::string greeting;
    
    NPC() = default;
    NPC(const std::string& n, const std::string& desc, const std::string& greet)
        : name(n), description(desc), greeting(greet) {}
};

class Room {
public:
    Room();
    Room(const std::string& id, const std::string& name, const std::string& desc);
    
    const std::string& getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    
    void setDescription(const std::string& desc) { description_ = desc; }
    
    void setExit(Direction dir, const std::string& roomId);
    std::string getExit(Direction dir) const;
    
    std::unordered_map<Direction, std::string> getExits() const { return exits_; }
    
    // NPC 管理
    void addNPC(const NPC& npc);
    void removeNPC(const std::string& name);
    const std::vector<NPC>& getNPCs() const { return npcs_; }
    
    // 怪物管理
    void addMonster(const Monster& monster);
    void removeMonster(const std::string& name);
    Monster* getMonster(const std::string& name);
    const Monster* getMonster(const std::string& name) const;
    std::vector<Monster>& getMonsters() { return monsters_; }
    const std::vector<Monster>& getMonsters() const { return monsters_; }
    bool hasMonsters() const { return !monsters_.empty() && monsters_[0].isAlive(); }
    
    static std::string directionToString(Direction dir);
    static Direction stringToDirection(const std::string& str);
    
private:
    std::string id_;
    std::string name_;
    std::string description_;
    std::unordered_map<Direction, std::string> exits_;
    std::vector<NPC> npcs_;
    std::vector<Monster> monsters_;
};

class World {
public:
    World();
    
    void addRoom(const Room& room);
    Room* getRoom(const std::string& id);
    const Room* getRoom(const std::string& id) const;
    
    std::string move(const std::string& currentRoomId, Direction dir);
    
private:
    std::unordered_map<std::string, Room> rooms_;
};

} // namespace mud

#endif // ROOM_H
