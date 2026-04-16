#include "command/MoveCommand.h"
#include "server/Session.h"
#include "world/Room.h"
#include "server/MudServer.h"
#include "combat/Combat.h"
#include <sstream>

namespace mud {

MoveCommand::MoveCommand(Direction dir) : direction_(dir) {
}

std::string MoveCommand::execute(Session* session, const std::vector<std::string>&) {
    if (session->isInCombat()) {
        return "\r\nYou cannot flee while in combat! Use 'flee' to try escaping!\r\n";
    }

    const Room* currentRoom = session->getCurrentRoom();
    if (!currentRoom) {
        return "\r\nYou are lost in nowhere...\r\n";
    }

    std::string nextRoomId = currentRoom->getExit(direction_);
    if (nextRoomId.empty()) {
        return "\r\nYou cannot go that way.\r\n";
    }

    std::string leave_msg = session->getPlayer().getName() + " leaves " +
                           Room::directionToString(direction_) + ".\r\n";
    session->getServer().broadcastToRoom(currentRoom->getId(), leave_msg, session->getId());

    session->setCurrentRoom(nextRoomId);
    const Room* newRoom = session->getCurrentRoom();

    if (!newRoom) {
        return "\r\nSomething went wrong. You are in nowhere.\r\n";
    }

    std::ostringstream oss;
    oss << "\r\nYou walk " << Room::directionToString(direction_) << " and arrive at "
        << newRoom->getName() << ".\r\n";

    std::string arrive_msg = session->getPlayer().getName() + " arrives from " +
                            oppositeDirectionToString(direction_) + ".\r\n";
    session->getServer().broadcastToRoom(newRoom->getId(), arrive_msg, session->getId());

    oss << "\r\n====== " << newRoom->getName() << " ======\r\n\r\n";
    oss << newRoom->getDescription() << "\r\n\r\n";

    auto exits = newRoom->getExits();
    if (!exits.empty()) {
        oss << "Visible exits: ";
        bool first = true;
        for (const auto& [dir, roomId] : exits) {
            if (!first) oss << ", ";
            oss << Room::directionToString(dir);
            first = false;
        }
        oss << "\r\n";
    }

    const auto& npcs = newRoom->getNPCs();
    if (!npcs.empty()) {
        oss << "\r\nYou see here:\r\n";
        for (const auto& npc : npcs) {
            oss << "  - " << npc.name << ": " << npc.description << "\r\n";
        }
    }

    const auto& monsters = newRoom->getMonsters();
    if (!monsters.empty()) {
        oss << "\r\n*** DANGEROUS CREATURES ***\r\n";
        for (const auto& monster : monsters) {
            if (monster.isAlive()) {
                oss << "  - " << monster.getName() << ": " << monster.getDescription() << "\r\n";
            }
        }
        oss << "\r\nType 'kill <monster>' to attack!\r\n";
    }

    auto playersInRoom = session->getServer().getPlayersInRoom(newRoom->getId(), session->getId());
    if (!playersInRoom.empty()) {
        oss << "\r\nOther adventurers here:\r\n";
        for (const auto& p : playersInRoom) {
            oss << "  - " << p << "\r\n";
        }
    }

    oss << "\r\n";
    return oss.str();
}

std::string MoveCommand::getHelp() const {
    return "Move in a direction (north/south/east/west)";
}

std::string MoveCommand::getName() const {
    return "move";
}

std::string MoveCommand::oppositeDirectionToString(Direction dir) {
    switch (dir) {
        case Direction::North: return "south";
        case Direction::South: return "north";
        case Direction::East:  return "west";
        case Direction::West:  return "east";
        case Direction::Up:    return "below";
        case Direction::Down:  return "above";
        case Direction::Enter: return "outside";
        case Direction::Exit:  return "inside";
        default: return "somewhere";
    }
}

} // namespace mud
