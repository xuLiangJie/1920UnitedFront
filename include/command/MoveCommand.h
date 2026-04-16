#ifndef MOVE_COMMAND_H
#define MOVE_COMMAND_H

#include "command/ICommand.h"
#include "world/Room.h"

namespace mud {

class MoveCommand : public ICommand {
public:
    MoveCommand(Direction dir);
    
    std::string execute(Session* session, const std::vector<std::string>& args) override;
    std::string getHelp() const override;
    std::string getName() const override;
    
private:
    std::string oppositeDirectionToString(Direction dir);
    
    Direction direction_;
};

} // namespace mud

#endif // MOVE_COMMAND_H
