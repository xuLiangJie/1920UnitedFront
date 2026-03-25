#include "world/Room.h"
#include <algorithm>

namespace mud {

Room::Room() = default;

Room::Room(const std::string& id, const std::string& name, const std::string& desc)
    : id_(id), name_(name), description_(desc) {
}

void Room::setExit(Direction dir, const std::string& roomId) {
    exits_[dir] = roomId;
}

std::string Room::getExit(Direction dir) const {
    auto it = exits_.find(dir);
    if (it != exits_.end()) {
        return it->second;
    }
    return "";
}

void Room::addNPC(const NPC& npc) {
    npcs_.push_back(npc);
}

void Room::removeNPC(const std::string& name) {
    npcs_.erase(
        std::remove_if(npcs_.begin(), npcs_.end(),
            [&name](const NPC& npc) { return npc.name == name; }),
        npcs_.end()
    );
}

void Room::addMonster(const Monster& monster) {
    monsters_.push_back(monster);
}

void Room::removeMonster(const std::string& name) {
    monsters_.erase(
        std::remove_if(monsters_.begin(), monsters_.end(),
            [&name](const Monster& m) { return m.getName() == name; }),
        monsters_.end()
    );
}

Monster* Room::getMonster(const std::string& name) {
    auto it = std::find_if(monsters_.begin(), monsters_.end(),
        [&name](const Monster& m) { return m.getName() == name; });
    if (it != monsters_.end()) {
        return &(*it);
    }
    return nullptr;
}

const Monster* Room::getMonster(const std::string& name) const {
    auto it = std::find_if(monsters_.begin(), monsters_.end(),
        [&name](const Monster& m) { return m.getName() == name; });
    if (it != monsters_.end()) {
        return &(*it);
    }
    return nullptr;
}

std::string Room::directionToString(Direction dir) {
    switch (dir) {
        case Direction::North: return "north";
        case Direction::South: return "south";
        case Direction::East:  return "east";
        case Direction::West:  return "west";
        case Direction::Up:    return "up";
        case Direction::Down:  return "down";
        case Direction::Enter: return "enter";
        case Direction::Exit:  return "exit";
        default: return "unknown";
    }
}

Direction Room::stringToDirection(const std::string& str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    
    if (lower == "north" || lower == "n") return Direction::North;
    if (lower == "south" || lower == "s") return Direction::South;
    if (lower == "east" || lower == "e")  return Direction::East;
    if (lower == "west" || lower == "w")  return Direction::West;
    if (lower == "up" || lower == "u")    return Direction::Up;
    if (lower == "down" || lower == "d")  return Direction::Down;
    if (lower == "enter" || lower == "in") return Direction::Enter;
    if (lower == "exit" || lower == "out") return Direction::Exit;
    return Direction::None;
}

// ==================== World ====================

World::World() {
    // 1. 起始大厅 - 安全区域，无怪物
    Room startingHall("hall", "Grand Starting Hall", 
        "You stand in the center of a grand stone hall. Ancient banners hang on the walls, "
        "depicting brave knights fighting dragons. The hall is bustling with adventurers. "
        "A corridor leads north, a small room is to the east, and the sound of flowing "
        "water comes from the west.");
    startingHall.setExit(Direction::North, "corridor");
    startingHall.setExit(Direction::East, "weapon_room");
    startingHall.setExit(Direction::West, "garden");
    startingHall.addNPC({"Elder Sage", "An old sage with a long white beard", 
                         "Welcome, young adventurer! The world awaits you."});
    addRoom(startingHall);
    
    // 2. 走廊 - 有哥布林
    Room corridor("corridor", "Crystal Corridor",
        "A long corridor with magical crystals glowing softly on the walls. "
        "The air is filled with mysterious energy. The starting hall is to the south, "
        "and a mysterious room lies to the north.");
    corridor.setExit(Direction::South, "hall");
    corridor.setExit(Direction::North, "treasure_room");
    corridor.addMonster(Monster::createGoblin());
    addRoom(corridor);
    
    // 3. 武器室 - 有野狼
    Room weaponRoom("weapon_room", "Weapon Room",
        "A small room with weapon racks on all sides. Most are empty, but you can "
        "still feel the former glory. An old weaponsmith stands in the corner. "
        "The exit is to the west.");
    weaponRoom.setExit(Direction::West, "hall");
    weaponRoom.addNPC({"Grom Ironhand", "A burly weaponsmith with soot-covered arms",
                       "Need a weapon? Ah, all the good ones are gone..."});
    weaponRoom.addMonster(Monster::createWolf());
    addRoom(weaponRoom);
    
    // 4. 喷泉花园 - 有骷髅
    Room garden("garden", "Fountain Garden",
        "A beautiful garden with an exquisite fountain in the center. "
        "Exotic flowers grow here, emitting a charming fragrance. "
        "A gentle breeze rustles the leaves. The main hall is to the east.");
    garden.setExit(Direction::East, "hall");
    garden.addNPC({"Luna", "A young woman in flowing robes, tending to the flowers",
                   "The flowers speak to those who listen..."});
    garden.addMonster(Monster::createSkeleton());
    addRoom(garden);
    
    // 5. 宝库 - 有兽人 Boss
    Room treasureRoom("treasure_room", "Mysterious Treasure Room",
        "A mysterious room with gems embedded in the walls, casting colorful reflections. "
        "An ancient treasure chest sits in the center, but it's firmly locked. "
        "Strange symbols cover the walls. The corridor is to the south.");
    treasureRoom.setExit(Direction::South, "corridor");
    treasureRoom.addMonster(Monster::createOrc());
    addRoom(treasureRoom);
}

void World::addRoom(const Room& room) {
    rooms_[room.getId()] = room;
}

Room* World::getRoom(const std::string& id) {
    auto it = rooms_.find(id);
    if (it != rooms_.end()) {
        return &it->second;
    }
    return nullptr;
}

const Room* World::getRoom(const std::string& id) const {
    auto it = rooms_.find(id);
    if (it != rooms_.end()) {
        return &it->second;
    }
    return nullptr;
}

std::string World::move(const std::string& currentRoomId, Direction dir) {
    auto it = rooms_.find(currentRoomId);
    if (it == rooms_.end()) {
        return "";
    }
    
    std::string nextRoomId = it->second.getExit(dir);
    if (nextRoomId.empty()) {
        return "";
    }
    
    return nextRoomId;
}

} // namespace mud
