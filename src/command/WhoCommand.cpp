#include "command/WhoCommand.h"
#include "server/Session.h"
#include "server/MudServer.h"
#include <sstream>

namespace mud {

std::string WhoCommand::execute(Session* session, const std::vector<std::string>&) {
    std::ostringstream oss;
    oss << "\r\n====== Online Players ======\r\n";

    auto& server = session->getServer();
    auto players = server.getOnlinePlayers();

    if (players.empty()) {
        oss << "No players online.\r\n";
    } else {
        for (const auto& player : players) {
            oss << "  - " << player << "\r\n";
        }
    }
    oss << "==========================\r\n";
    return oss.str();
}

std::string WhoCommand::getHelp() const {
    return "who - List online players";
}

std::string WhoCommand::getName() const {
    return "who";
}

} // namespace mud
