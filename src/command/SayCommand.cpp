#include "command/SayCommand.h"
#include "server/Session.h"
#include "server/MudServer.h"
#include "player/Player.h"
#include "world/Room.h"
#include <sstream>

namespace mud {

std::string SayCommand::execute(Session* session, const std::vector<std::string>& args) {
    if (args.size() < 2) {
        return "Usage: say <text>\r\n";
    }

    auto& player = session->getPlayer();
    std::ostringstream oss;

    std::string message;
    for (size_t i = 1; i < args.size(); ++i) {
        if (i > 1) message += " ";
        message += args[i];
    }

    oss << "\r\nYou say: \"" << message << "\"\r\n";

    const Room* room = session->getCurrentRoom();
    if (room) {
        std::string broadcast_msg = player.getName() + " says: \"" + message + "\"\r\n";
        session->getServer().broadcastToRoom(room->getId(), broadcast_msg, session->getId());
    }

    return oss.str();
}

std::string SayCommand::getHelp() const {
    return "say <text> - Say something to people in same room";
}

std::string SayCommand::getName() const {
    return "say";
}

} // namespace mud
