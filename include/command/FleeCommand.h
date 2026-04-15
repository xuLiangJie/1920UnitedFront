#ifndef FLEE_COMMAND_H
#define FLEE_COMMAND_H

#include "command/ICommand.h"

namespace mud {

class FleeCommand : public ICommand {
public:
    std::string execute(Session* session, const std::vector<std::string>& args) override;
    std::string getHelp() const override;
    std::string getName() const override;
};

} // namespace mud

#endif // FLEE_COMMAND_H
