#ifndef DELETE_COMMAND_H
#define DELETE_COMMAND_H

#include "command/ICommand.h"

namespace mud {

class DeleteCommand : public ICommand {
public:
    std::string execute(Session* session, const std::vector<std::string>& args) override;
    std::string getHelp() const override;
    std::string getName() const override;
};

} // namespace mud

#endif // DELETE_COMMAND_H
