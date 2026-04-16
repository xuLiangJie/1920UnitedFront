#include "command/NameCommand.h"
#include "server/Session.h"
#include "player/Player.h"
#include "server/MudServer.h"
#include "world/Room.h"
#include <sstream>

namespace mud {

std::string NameCommand::execute(Session* session, const std::vector<std::string>& args) {
    if (args.size() < 2) {
        return "Usage: name <character_name>\r\n";
    }

    auto& player = session->getPlayer();
    std::string new_name = args[1];

    if (new_name.length() < 2 || new_name.length() > 20) {
        return "Name must be between 2-20 characters.\r\n";
    }

    std::string old_name = player.getName();
    player.setName(new_name);
    player.setLoggedIn(true);

    std::ostringstream oss;
    oss << "\r\nYour name has been changed from \"" << old_name << "\" to \"" << new_name << "\".\r\n";

    const Room* room = session->getCurrentRoom();
    if (room) {
        session->getServer().broadcastToRoom(room->getId(),
            player.getName() + " has entered the game.\r\n", session->getId());
    }

    return oss.str();
}

std::string NameCommand::getHelp() const {
    return "name <name> - Set your character name";
}

std::string NameCommand::getName() const {
    return "name";
}

} // namespace mud
