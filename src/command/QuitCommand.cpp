#include "command/QuitCommand.h"
#include "server/Session.h"
#include "server/MudServer.h"
#include "player/Player.h"
#include "world/Room.h"
#include "save/SaveManager.h"
#include <sstream>

namespace mud {

std::string QuitCommand::execute(Session* session, const std::vector<std::string>&) {
    auto& player = session->getPlayer();
    const Room* room = session->getCurrentRoom();

    // 自动保存
    std::string filename = SaveManager::generateFilename(player.getName());
    if (SaveManager::getInstance().savePlayer(filename, player.toData())) {
        std::ostringstream oss;
        oss << "\r\n[Auto-Save] Game saved as '" << filename << "'\r\n";
        session->send(oss.str());
    }

    std::ostringstream oss;
    oss << "\r\nThank you for playing, " << player.getName() << "! See you again.\r\n";
    oss << "Quitting game...\r\n";

    if (room) {
        session->getServer().broadcastToRoom(room->getId(),
            player.getName() + " has left the game.\r\n", session->getId());
    }

    session->stop();

    return oss.str();
}

std::string QuitCommand::getHelp() const {
    return "quit - Quit the game (auto-saves)";
}

std::string QuitCommand::getName() const {
    return "quit";
}

} // namespace mud
