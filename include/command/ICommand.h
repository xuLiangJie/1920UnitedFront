#ifndef ICOMMAND_H
#define ICOMMAND_H

#include <string>
#include <vector>
#include <memory>

namespace mud {

class Session;

// 命令接口 - 命令模式
class ICommand {
public:
    virtual ~ICommand() = default;
    
    // 执行命令
    virtual std::string execute(Session* session, const std::vector<std::string>& args) = 0;
    
    // 获取命令帮助
    virtual std::string getHelp() const = 0;
    
    // 获取命令名称
    virtual std::string getName() const = 0;
};

} // namespace mud

#endif // ICOMMAND_H
