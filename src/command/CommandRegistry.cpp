#include "command/CommandRegistry.h"
#include "server/Session.h"
#include <sstream>
#include <algorithm>
#include <cctype>

namespace mud {

CommandRegistry::CommandRegistry() {
}

CommandRegistry& CommandRegistry::getInstance() {
    static CommandRegistry instance;
    return instance;
}

void CommandRegistry::registerCommand(std::shared_ptr<ICommand> command) {
    std::string name = command->getName();
    std::transform(name.begin(), name.end(), name.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    commands_[name] = std::move(command);
}

void CommandRegistry::registerCommandWithAlias(std::shared_ptr<ICommand> command,
                                                 const std::vector<std::string>& aliases) {
    for (const auto& alias : aliases) {
        commands_[alias] = command;
    }
}

std::string CommandRegistry::executeCommand(Session* session, const std::string& input) {
    if (input.empty()) {
        return "";
    }
    
    // 解析输入
    std::istringstream iss(input);
    std::vector<std::string> args;
    std::string arg;
    
    while (iss >> arg) {
        args.push_back(arg);
    }
    
    if (args.empty()) {
        return "";
    }
    
    // 转换为小写
    std::string cmd = args[0];
    std::transform(cmd.begin(), cmd.end(), cmd.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    
    // 查找并执行命令
    auto it = commands_.find(cmd);
    if (it != commands_.end()) {
        return it->second->execute(session, args);
    } else {
        return "Unknown command. Type 'help' to see available commands.\r\n";
    }
}

std::string CommandRegistry::getAllHelp() const {
    std::ostringstream oss;
    oss << "\r\n====== Western Fantasy MUD - Command Help ======\r\n\r\n";
    oss << "[Basic Commands]\r\n";
    oss << "  help          - Show this help message\r\n";
    oss << "  who           - List online players\r\n";
    oss << "  look          - Look around (see room, NPCs, monsters)\r\n";
    oss << "  score         - Show your status\r\n";
    oss << "  name <name>   - Set your character name\r\n";
    oss << "  say <text>    - Say something to people in same room\r\n";
    oss << "\r\n";
    oss << "[Movement Commands]\r\n";
    oss << "  north (n)     - Move north\r\n";
    oss << "  south (s)     - Move south\r\n";
    oss << "  east (e)      - Move east\r\n";
    oss << "  west (w)      - Move west\r\n";
    oss << "\r\n";
    oss << "[Combat Commands]\r\n";
    oss << "  kill <monster>- Attack a monster\r\n";
    oss << "  attack        - Continue attacking current enemy\r\n";
    oss << "  flee          - Try to escape from combat\r\n";
    oss << "\r\n";
    oss << "[Save System]\r\n";
    oss << "  save          - Save your character progress\r\n";
    oss << "  load          - Load a saved character\r\n";
    oss << "  saves         - List all saved characters\r\n";
    oss << "  delete <name> - Delete a saved character\r\n";
    oss << "\r\n";
    oss << "[Other Commands]\r\n";
    oss << "  inventory (i) - Check your inventory\r\n";
    oss << "  quit          - Quit the game (auto-saves)\r\n";
    oss << "\r\n";
    oss << "====================================================\r\n";
    return oss.str();
}

bool CommandRegistry::hasCommand(const std::string& name) const {
    std::string lower = name;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return commands_.find(lower) != commands_.end();
}

} // namespace mud
