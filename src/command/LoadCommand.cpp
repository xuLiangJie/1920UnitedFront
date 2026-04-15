#include "command/LoadCommand.h"
#include "server/Session.h"
#include "player/Player.h"
#include "save/SaveManager.h"
#include "world/Room.h"
#include <sstream>

namespace mud {

std::string LoadCommand::execute(Session* session, const std::vector<std::string>& args) {
    std::string characterName;

    if (args.size() >= 2) {
        characterName = args[1];
    } else {
        // 使用当前角色名
        characterName = session->getPlayer().getName();
        if (characterName == "Stranger") {
            return "\r\nUsage: load <character_name>\r\n"
                   "Or set your name first with 'name <yourname>'\r\n";
        }
    }

    std::string filename = SaveManager::generateFilename(characterName);

    if (!SaveManager::getInstance().hasSave(filename)) {
        std::ostringstream oss;
        oss << "\r\n[Error] No save file found for '" << characterName << "'\r\n";
        oss << "Use 'saves' to see all saved characters.\r\n";
        return oss.str();
    }

    PlayerData data;
    if (!SaveManager::getInstance().loadPlayer(filename, data)) {
        return "\r\n[Error] Failed to load save file. It may be corrupted.\r\n";
    }

    // 应用加载的数据
    auto& player = session->getPlayer();
    player.fromData(data);
    session->setCurrentRoom(data.location);

    std::ostringstream oss;
    oss << "\r\n[Load] Welcome back, " << player.getName() << "!\r\n";
    oss << "Loaded character:\r\n";
    oss << "  Level: " << player.getLevel() << "\r\n";
    oss << "  HP: " << player.getCurrentHP() << "/" << player.getMaxHP() << "\r\n";
    oss << "  MP: " << player.getCurrentMP() << "/" << player.getMaxMP() << "\r\n";
    oss << "  Gold: " << player.getGold() << "\r\n";
    oss << "  Items: " << player.getInventory().size() << "\r\n";
    oss << "\r\nYou are now at " << session->getCurrentRoom()->getName() << ".\r\n";

    return oss.str();
}

std::string LoadCommand::getHelp() const {
    return "load - Load a saved character";
}

std::string LoadCommand::getName() const {
    return "load";
}

} // namespace mud
