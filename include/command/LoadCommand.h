#ifndef LOAD_COMMAND_H
#define LOAD_COMMAND_H

#include "command/ICommand.h"

namespace mud {

class LoadCommand : public ICommand {
public:
    std::string execute(Session* session, const std::vector<std::string>& args) override;
    std::string getHelp() const override;
    std::string getName() const override;
};

} // namespace mud

#endif // LOAD_COMMAND_H
