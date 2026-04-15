#include "command/LookCommand.h"
#include "server/Session.h"
#include "world/Room.h"
#include "server/MudServer.h"
#include <sstream>

namespace mud {

std::string LookCommand::execute(Session* session, const std::vector<std::string>&) {
    const Room* room = session->getCurrentRoom();
    if (!room) {
        return "\r\nYou are in nowhere. Something is wrong...\r\n";
    }

    std::ostringstream oss;
    oss << "\r\n====== " << room->getName() << " ======\r\n\r\n";
    oss << room->getDescription() << "\r\n\r\n";

    auto exits = room->getExits();
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

    const auto& npcs = room->getNPCs();
    if (!npcs.empty()) {
        oss << "\r\nYou see here:\r\n";
        for (const auto& npc : npcs) {
            oss << "  - " << npc.name << ": " << npc.description << "\r\n";
        }
    }

    const auto& monsters = room->getMonsters();
    if (!monsters.empty()) {
        oss << "\r\n*** DANGEROUS CREATURES ***\r\n";
        for (const auto& monster : monsters) {
            if (monster.isAlive()) {
                oss << "  - " << monster.getName() << ": " << monster.getDescription() << "\r\n";
                oss << "    " << monster.getHealthStatus() << "\r\n";
            }
        }
        oss << "\r\nType 'kill <monster>' to attack!\r\n";
    }

    auto& server = session->getServer();
    auto playersInRoom = server.getPlayersInRoom(room->getId(), session->getId());
    if (!playersInRoom.empty()) {
        oss << "\r\nOther adventurers here:\r\n";
        for (const auto& player : playersInRoom) {
            oss << "  - " << player << "\r\n";
        }
    }

    oss << "\r\n";
    return oss.str();
}

std::string LookCommand::getHelp() const {
    return "look - Look around (see room, NPCs, monsters)";
}

std::string LookCommand::getName() const {
    return "look";
}

} // namespace mud
