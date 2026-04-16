#include "command/SaveCommand.h"
#include "server/Session.h"
#include "player/Player.h"
#include "save/SaveManager.h"
#include <sstream>

namespace mud {

std::string SaveCommand::execute(Session* session, const std::vector<std::string>&) {
    auto& player = session->getPlayer();

    if (player.getName() == "Stranger") {
        return "\r\nYou need to set a name first! Use 'name <yourname>'\r\n";
    }

    std::string filename = SaveManager::generateFilename(player.getName());

    if (SaveManager::getInstance().savePlayer(filename, player.toData())) {
        std::ostringstream oss;
        oss << "\r\n[Save] Game saved successfully as '" << filename << "'\r\n";
        oss << "Your progress has been preserved!\r\n";
        return oss.str();
    } else {
        return "\r\n[Error] Failed to save game. Please try again.\r\n";
    }
}

std::string SaveCommand::getHelp() const {
    return "save - Save your character progress";
}

std::string SaveCommand::getName() const {
    return "save";
}

} // namespace mud
