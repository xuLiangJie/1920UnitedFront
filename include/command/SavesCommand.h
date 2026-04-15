#ifndef SAVES_COMMAND_H
#define SAVES_COMMAND_H

#include "command/ICommand.h"

namespace mud {

class SavesCommand : public ICommand {
public:
    std::string execute(Session* session, const std::vector<std::string>& args) override;
    std::string getHelp() const override;
    std::string getName() const override;
};

} // namespace mud

#endif // SAVES_COMMAND_H
