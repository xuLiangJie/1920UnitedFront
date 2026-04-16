#include "command/InventoryCommand.h"
#include "server/Session.h"
#include "player/Player.h"
#include <sstream>

namespace mud {

std::string InventoryCommand::execute(Session* session, const std::vector<std::string>&) {
    auto& player = session->getPlayer();
    const auto& inventory = player.getInventory();

    std::ostringstream oss;
    oss << "\r\n====== Inventory ======\r\n";

    if (inventory.empty()) {
        oss << "You are not carrying anything.\r\n";
    } else {
        oss << "You are carrying:\r\n";
        for (const auto& item : inventory) {
            oss << "  - " << item.name << ": " << item.description << "\r\n";
        }
    }
    oss << "=======================\r\n";

    return oss.str();
}

std::string InventoryCommand::getHelp() const {
    return "inventory (i) - Check your inventory";
}

std::string InventoryCommand::getName() const {
    return "inventory";
}

} // namespace mud
