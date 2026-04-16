#ifndef QUIT_COMMAND_H
#define QUIT_COMMAND_H

#include "command/ICommand.h"

namespace mud {

class QuitCommand : public ICommand {
public:
    std::string execute(Session* session, const std::vector<std::string>& args) override;
    std::string getHelp() const override;
    std::string getName() const override;
};

} // namespace mud

#endif // QUIT_COMMAND_H
