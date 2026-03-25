#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <string>
#include <unordered_map>
#include <functional>
#include <vector>
#include "world/Room.h"

namespace mud {

class Session;

class CommandHandler {
public:
    CommandHandler();
    
    std::string handleCommand(Session* session, const std::string& input);
    
private:
    // 基本命令
    std::string cmdHelp(Session* session, const std::vector<std::string>& args);
    std::string cmdWho(Session* session, const std::vector<std::string>& args);
    std::string cmdLook(Session* session, const std::vector<std::string>& args);
    std::string cmdSay(Session* session, const std::vector<std::string>& args);
    std::string cmdName(Session* session, const std::vector<std::string>& args);
    std::string cmdScore(Session* session, const std::vector<std::string>& args);
    std::string cmdQuit(Session* session, const std::vector<std::string>& args);
    std::string cmdMove(Session* session, const std::vector<std::string>& args, Direction dir);
    std::string cmdInventory(Session* session, const std::vector<std::string>& args);
    
    // 战斗命令
    std::string cmdKill(Session* session, const std::vector<std::string>& args);
    std::string cmdAttack(Session* session, const std::vector<std::string>& args);
    std::string cmdFlee(Session* session, const std::vector<std::string>& args);
    
    std::string oppositeDirectionToString(Direction dir);
    std::vector<std::string> parseInput(const std::string& input);
    
    std::unordered_map<std::string, 
        std::function<std::string(Session*, const std::vector<std::string>&)>> commands_;
};

} // namespace mud

#endif // COMMAND_HANDLER_H
