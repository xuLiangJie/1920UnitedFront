#ifndef COMMAND_REGISTRY_H
#define COMMAND_REGISTRY_H

#include "command/ICommand.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace mud {

class Session;

// 命令注册中心 - 管理所有命令的注册和执行
class CommandRegistry {
public:
    static CommandRegistry& getInstance();
    
    // 注册命令
    void registerCommand(std::shared_ptr<ICommand> command);
    
    // 注册命令(带别名)
    void registerCommandWithAlias(std::shared_ptr<ICommand> command, 
                                   const std::vector<std::string>& aliases);
    
    // 执行命令
    std::string executeCommand(Session* session, const std::string& input);
    
    // 获取所有命令帮助
    std::string getAllHelp() const;
    
    // 检查命令是否存在
    bool hasCommand(const std::string& name) const;
    
private:
    CommandRegistry();
    
    std::unordered_map<std::string, std::shared_ptr<ICommand>> commands_;
};

} // namespace mud

#endif // COMMAND_REGISTRY_H
