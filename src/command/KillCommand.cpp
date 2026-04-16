#include "command/KillCommand.h"
#include "server/Session.h"
#include "world/Room.h"
#include "combat/Monster.h"
#include "combat/Combat.h"
#include <cctype>

namespace mud {

std::string KillCommand::execute(Session* session, const std::vector<std::string>& args) {
    if (args.size() < 2) {
        return "Usage: kill <monster_name>\r\nType 'look' to see what monsters are here.\r\n";
    }

    if (session->isInCombat()) {
        return "\r\nYou are already in combat! Use 'attack' to continue fighting,\r\n"
               "or 'flee' to try to escape!\r\n";
    }

    Room* room = session->getCurrentRoom();
    if (!room) {
        return "\r\nYou are in nowhere...\r\n";
    }

    // 组合所有参数作为怪物名（支持多词名字如 "Skeleton Warrior"）
    std::string monsterName;
    for (size_t i = 1; i < args.size(); ++i) {
        if (i > 1) monsterName += " ";
        monsterName += args[i];
    }

    // 首字母大写
    if (!monsterName.empty()) {
        monsterName[0] = std::toupper(monsterName[0]);
    }

    Monster* monster = room->getMonster(monsterName);
    if (!monster || !monster->isAlive()) {
        return "\r\nThere is no such monster here.\r\n";
    }

    session->getCombatManager().startCombat(session->getId(), *monster);
    room->removeMonster(monsterName);

    return "";
}

std::string KillCommand::getHelp() const {
    return "kill <monster> - Attack a monster";
}

std::string KillCommand::getName() const {
    return "kill";
}

} // namespace mud
