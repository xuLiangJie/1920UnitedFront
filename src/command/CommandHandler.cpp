#include "command/CommandHandler.h"

namespace mud {

void CommandHandler::initializeCommands() {
    auto& registry = CommandRegistry::getInstance();
    
    // 基础命令
    registry.registerCommand(std::make_shared<HelpCommand>());
    registry.registerCommand(std::make_shared<LookCommand>());
    registry.registerCommand(std::make_shared<WhoCommand>());
    registry.registerCommand(std::make_shared<ScoreCommand>());
    registry.registerCommand(std::make_shared<NameCommand>());
    registry.registerCommand(std::make_shared<SayCommand>());
    registry.registerCommand(std::make_shared<QuitCommand>());
    registry.registerCommand(std::make_shared<InventoryCommand>());
    
    // 移动命令(带别名)
    auto moveNorth = std::make_shared<MoveCommand>(Direction::North);
    registry.registerCommandWithAlias(moveNorth, {"north", "n"});
    
    auto moveSouth = std::make_shared<MoveCommand>(Direction::South);
    registry.registerCommandWithAlias(moveSouth, {"south", "s"});
    
    auto moveEast = std::make_shared<MoveCommand>(Direction::East);
    registry.registerCommandWithAlias(moveEast, {"east", "e"});
    
    auto moveWest = std::make_shared<MoveCommand>(Direction::West);
    registry.registerCommandWithAlias(moveWest, {"west", "w"});
    
    // 战斗命令
    registry.registerCommand(std::make_shared<KillCommand>());
    registry.registerCommand(std::make_shared<AttackCommand>());
    registry.registerCommand(std::make_shared<FleeCommand>());
    
    // 存档命令
    registry.registerCommand(std::make_shared<SaveCommand>());
    registry.registerCommand(std::make_shared<LoadCommand>());
    registry.registerCommand(std::make_shared<DeleteCommand>());
    registry.registerCommand(std::make_shared<SavesCommand>());
}

} // namespace mud
