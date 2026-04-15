#ifndef KILL_COMMAND_H
#define KILL_COMMAND_H

#include "command/ICommand.h"

namespace mud {

class KillCommand : public ICommand {
public:
    std::string execute(Session* session, const std::vector<std::string>& args) override;
    std::string getHelp() const override;
    std::string getName() const override;
};

} // namespace mud

#endif // KILL_COMMAND_H
