#ifndef NAME_COMMAND_H
#define NAME_COMMAND_H

#include "command/ICommand.h"

namespace mud {

class NameCommand : public ICommand {
public:
    std::string execute(Session* session, const std::vector<std::string>& args) override;
    std::string getHelp() const override;
    std::string getName() const override;
};

} // namespace mud

#endif // NAME_COMMAND_H
