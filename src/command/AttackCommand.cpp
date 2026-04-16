#include "command/AttackCommand.h"
#include "server/Session.h"
#include "combat/Combat.h"
#include "player/Player.h"
#include <sstream>

namespace mud {

std::string AttackCommand::execute(Session* session, const std::vector<std::string>&) {
    if (!session->isInCombat()) {
        return "\r\nYou are not in combat. Use 'kill <monster>' to start fighting!\r\n";
    }

    auto& player = session->getPlayer();
    CombatResult result = session->getCombatManager().playerAttack(
        session->getId(),
        player.getAttack(),
        player.getDefense(),
        player.getCurrentHP()
    );

    // 如果战斗胜利，添加经验值
    if (result.playerWon && result.expGained > 0) {
        player.addExp(result.expGained);

        std::ostringstream oss;
        oss << result.message;
        oss << "\r\n[Level Up!] You are now level " << player.getLevel() << "!\r\n";
        oss << "HP and MP restored, Attack and Defense increased!\r\n";
        return oss.str();
    }

    return result.message;
}

std::string AttackCommand::getHelp() const {
    return "attack - Continue attacking current enemy";
}

std::string AttackCommand::getName() const {
    return "attack";
}

} // namespace mud
