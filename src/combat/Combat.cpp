#include "combat/Combat.h"
#include "strategy/DamageStrategy.h"
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <memory>

namespace mud {

CombatManager::CombatManager()
    : state_(CombatState::NotInCombat) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

void CombatManager::startCombat(uint32_t sessionId, Monster monster) {
    monster_ = monster;
    state_ = CombatState::InCombat;

    // 发布战斗开始事件
    CombatStartedEvent event(sessionId, monster_.getName(), monster_.getDescription());
    EventBus::getInstance().publish(event);
}

CombatResult CombatManager::playerAttack(uint32_t sessionId, int playerAttack, int playerDefense, int playerHP) {
    CombatResult result;
    result.sessionId = sessionId;

    if (state_ != CombatState::InCombat || !monster_.isAlive()) {
        result.message = "You are not in combat.\r\n";
        return result;
    }

    // 使用策略模式计算伤害
    PhysicalDamageStrategy strategy;
    bool isCrit = false;
    int damage = strategy.calculateDamage(playerAttack, monster_.getDefense(), isCrit);

    // 应用伤害
    monster_.takeDamage(damage);
    result.damageDealt = damage;

    // 构建消息
    std::ostringstream oss;
    oss << "\r\nYou attack the " << monster_.getName() << " ";
    if (isCrit) {
        oss << "with a CRITICAL HIT ";
    }
    oss << "and deal " << damage << " damage!\r\n";

    // 怪物状态
    oss << monster_.getHealthStatus() << "\r\n";

    result.message = oss.str();

    // 发布玩家攻击事件
    PlayerAttackedEvent event(sessionId, damage, isCrit, monster_.getName());
    EventBus::getInstance().publish(event);

    // 检查怪物是否死亡
    if (monster_.isDead()) {
        result.playerWon = true;
        result.expGained = monster_.getExpReward();

        oss.str("");
        oss << "\r\n*** VICTORY! ***\r\n";
        oss << "You have defeated the " << monster_.getName() << "!\r\n";
        oss << "You gained " << result.expGained << " experience points!\r\n";
        result.message = oss.str();

        endCombat(sessionId, true);
    } else {
        // 怪物反击
        CombatResult monsterResult = monsterAttack(sessionId, playerDefense, playerHP);
        result.damageReceived = monsterResult.damageDealt;
        result.message += monsterResult.message;

        // 检查玩家是否死亡
        if (playerHP - result.damageReceived <= 0) {
            result.playerWon = false;
            endCombat(sessionId, false);
        }
    }

    return result;
}

CombatResult CombatManager::monsterAttack(uint32_t sessionId, int playerDefense, int playerHP) {
    CombatResult result;
    result.sessionId = sessionId;

    if (state_ != CombatState::InCombat || !monster_.isAlive()) {
        return result;
    }

    // 怪物攻击
    int damage = monster_.attack();

    // 玩家防御减免
    int actual_damage = std::max(1, damage - playerDefense);

    result.damageDealt = actual_damage;

    std::ostringstream oss;
    oss << "\r\nThe " << monster_.getName() << " attacks you and deals "
        << actual_damage << " damage!\r\n";

    // 玩家 HP 状态
    int remaining_hp = playerHP - actual_damage;
    if (remaining_hp > 80) {
        oss << "You barely feel the wound.\r\n";
    } else if (remaining_hp > 60) {
        oss << "You take a minor hit.\r\n";
    } else if (remaining_hp > 40) {
        oss << "You are starting to feel the pain.\r\n";
    } else if (remaining_hp > 20) {
        oss << "You are badly hurt! Be careful!\r\n";
    } else {
        oss << "You are on the brink of death! Flee if you can!\r\n";
    }

    result.message = oss.str();

    // 发布怪物攻击事件
    MonsterAttackedEvent event(sessionId, actual_damage, monster_.getName());
    EventBus::getInstance().publish(event);

    return result;
}

void CombatManager::endCombat(uint32_t sessionId, bool playerWon) {
    if (playerWon) {
        state_ = CombatState::CombatWon;
    } else {
        state_ = CombatState::CombatLost;

        std::ostringstream oss;
        oss << "\r\n*** DEFEAT ***\r\n";
        oss << "You have been defeated by the " << monster_.getName() << "...\r\n";
        oss << "You black out and wake up back at the Starting Hall.\r\n";

        // 发布玩家死亡事件
        PlayerDiedEvent event(sessionId);
        EventBus::getInstance().publish(event);
    }

    // 发布战斗结束事件
    int exp = playerWon ? monster_.getExpReward() : 0;
    CombatEndedEvent event(sessionId, playerWon, exp, monster_.getName());
    EventBus::getInstance().publish(event);

    // 重置战斗状态
    state_ = CombatState::NotInCombat;
}

std::string CombatManager::getCombatStatus() const {
    if (state_ != CombatState::InCombat) {
        return "You are not in combat.\r\n";
    }

    std::ostringstream oss;
    oss << "You are fighting a " << monster_.getName() << ".\r\n";
    oss << monster_.getHealthStatus() << "\r\n";
    return oss.str();
}

} // namespace mud
