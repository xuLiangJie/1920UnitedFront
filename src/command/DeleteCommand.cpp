#include "command/DeleteCommand.h"
#include "server/Session.h"
#include "save/SaveManager.h"
#include <sstream>

namespace mud {

std::string DeleteCommand::execute(Session* session, const std::vector<std::string>& args) {
    if (args.size() < 2) {
        return "\r\nUsage: delete <character_name>\r\n"
               "Warning: This will permanently delete the save file!\r\n";
    }

    std::string characterName = args[1];
    std::string filename = SaveManager::generateFilename(characterName);

    if (!SaveManager::getInstance().hasSave(filename)) {
        std::ostringstream oss;
        oss << "\r\n[Error] No save file found for '" << characterName << "'\r\n";
        return oss.str();
    }

    if (SaveManager::getInstance().deleteSave(filename)) {
        std::ostringstream oss;
        oss << "\r\n[Delete] Save file '" << filename << "' has been deleted.\r\n";
        return oss.str();
    } else {
        return "\r\n[Error] Failed to delete save file.\r\n";
    }
}

std::string DeleteCommand::getHelp() const {
    return "delete <name> - Delete a saved character";
}

std::string DeleteCommand::getName() const {
    return "delete";
}

} // namespace mud
