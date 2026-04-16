#include "command/HelpCommand.h"
#include "server/Session.h"
#include "command/CommandRegistry.h"

namespace mud {

std::string HelpCommand::execute(Session* session, const std::vector<std::string>&) {
    return CommandRegistry::getInstance().getAllHelp();
}

std::string HelpCommand::getHelp() const {
    return "help - Show this help message";
}

std::string HelpCommand::getName() const {
    return "help";
}

} // namespace mud
