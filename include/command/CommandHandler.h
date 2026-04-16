#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "command/CommandRegistry.h"
#include "command/HelpCommand.h"
#include "command/LookCommand.h"
#include "command/WhoCommand.h"
#include "command/ScoreCommand.h"
#include "command/NameCommand.h"
#include "command/SayCommand.h"
#include "command/QuitCommand.h"
#include "command/InventoryCommand.h"
#include "command/MoveCommand.h"
#include "command/KillCommand.h"
#include "command/AttackCommand.h"
#include "command/FleeCommand.h"
#include "command/SaveCommand.h"
#include "command/LoadCommand.h"
#include "command/DeleteCommand.h"
#include "command/SavesCommand.h"
#include "world/Room.h"
#include <memory>

namespace mud {

// 命令处理器 - 现在仅作为命令注册初始化器
// 所有命令逻辑已移至独立的命令类中
class CommandHandler {
public:
    CommandHandler() {
        initializeCommands();
    }

    // 向后兼容:委托给CommandRegistry
    std::string handleCommand(Session* session, const std::string& input) {
        return CommandRegistry::getInstance().executeCommand(session, input);
    }
    
private:
    void initializeCommands();
};

} // namespace mud

#endif // COMMAND_HANDLER_H
