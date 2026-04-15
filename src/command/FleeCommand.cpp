#include "command/FleeCommand.h"
#include "server/Session.h"
#include "world/Room.h"
#include "combat/Combat.h"
#include <sstream>
#include <cstdlib>

namespace mud {

std::string FleeCommand::execute(Session* session, const std::vector<std::string>&) {
    if (!session->isInCombat()) {
        return "\r\nYou are not in combat.\r\n";
    }

    bool success = (rand() % 100) < 50;

    std::ostringstream oss;
    if (success) {
        oss << "\r\nYou manage to escape from the fight!\r\n";

        session->setCurrentRoom("hall");
        session->getCombatManager().endCombat(session->getId(), false);

        const Room* newRoom = session->getCurrentRoom();
        oss << "You arrive at " << newRoom->getName() << ", panting heavily.\r\n";
    } else {
        oss << "\r\nYou try to flee, but the monster blocks your path!\r\n";

        auto& player = session->getPlayer();
        CombatResult result = session->getCombatManager().monsterAttack(
            session->getId(),
            player.getDefense(),
            player.getCurrentHP()
        );
        oss << result.message;
    }

    return oss.str();
}

std::string FleeCommand::getHelp() const {
    return "flee - Try to escape from combat";
}

std::string FleeCommand::getName() const {
    return "flee";
}

} // namespace mud
