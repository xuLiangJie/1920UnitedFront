#include "command/SavesCommand.h"
#include "save/SaveManager.h"
#include <sstream>

namespace mud {

std::string SavesCommand::execute(Session* session, const std::vector<std::string>&) {
    auto saves = SaveManager::getInstance().getSaveList();

    std::ostringstream oss;
    oss << "\r\n====== Saved Characters ======\r\n";

    if (saves.empty()) {
        oss << "No saved characters found.\r\n";
    } else {
        for (const auto& save : saves) {
            oss << "  - " << save << "\r\n";
        }
    }
    oss << "==============================\r\n";
    oss << "Use 'load <name>' to load a character.\r\n";
    oss << "Use 'delete <name>' to delete a save.\r\n";

    return oss.str();
}

std::string SavesCommand::getHelp() const {
    return "saves - List all saved characters";
}

std::string SavesCommand::getName() const {
    return "saves";
}

} // namespace mud
