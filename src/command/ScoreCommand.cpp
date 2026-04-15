#include "command/ScoreCommand.h"
#include "server/Session.h"
#include "player/Player.h"
#include "world/Room.h"
#include <sstream>

namespace mud {

std::string ScoreCommand::execute(Session* session, const std::vector<std::string>&) {
    auto& player = session->getPlayer();
    const Room* room = session->getCurrentRoom();

    std::ostringstream oss;
    oss << "\r\n====== " << player.getName() << "'s Status ======\r\n";
    oss << "  Level: " << player.getLevel() << "\r\n";
    oss << "  Experience: " << player.getExp() << "\r\n";
    oss << "  Location: " << (room ? room->getName() : "Unknown") << "\r\n";
    oss << "  HP: " << player.getCurrentHP() << "/" << player.getMaxHP() << "\r\n";
    oss << "  MP: " << player.getCurrentMP() << "/" << player.getMaxMP() << "\r\n";
    oss << "  Attack: " << player.getAttack() << "\r\n";
    oss << "  Defense: " << player.getDefense() << "\r\n";
    oss << "  Gold: " << player.getGold() << "\r\n";
    oss << "  Items: " << player.getInventory().size() << "\r\n";
    oss << "============================================\r\n";

    return oss.str();
}

std::string ScoreCommand::getHelp() const {
    return "score - Show your status";
}

std::string ScoreCommand::getName() const {
    return "score";
}

} // namespace mud
