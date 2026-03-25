#include "combat/Combat.h"
#include "server/Session.h"
#include "player/Player.h"
#include <sstream>
#include <cstdlib>
#include <ctime>

namespace mud {

CombatManager::CombatManager() 
    : state_(CombatState::NotInCombat) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
}

void CombatManager::startCombat(Session* session, Monster monster) {
    monster_ = monster;
    state_ = CombatState::InCombat;
    
    std::ostringstream oss;
    oss << "\r\n====== COMBAT STARTED ======\r\n\r\n";
    oss << "A " << monster_.getName() << " appears before you!\r\n";
    oss << monster_.getDescription() << "\r\n\r\n";
    oss << monster_.getHealthStatus() << "\r\n";
    oss << "\r\nType 'kill " << monster_.getName() << "' or 'attack' to fight!\r\n";
    oss << "============================\r\n";
    
    session->send(oss.str());
}

CombatResult CombatManager::playerAttack(Session* session) {
    CombatResult result;
    
    if (state_ != CombatState::InCombat || !monster_.isAlive()) {
        result.message = "You are not in combat.\r\n";
        return result;
    }
    
    auto& player = session->getPlayer();
    
    // 玩家属性（简化版）
    int player_attack = 8 + rand() % 4;  // 基础攻击 8-11
    int player_level = 1;
    
    // 计算伤害
    int damage = std::max(1, player_attack - monster_.getDefense() / 2);
    
    // 暴击 chance (10%)
    bool isCrit = (rand() % 100) < 10;
    if (isCrit) {
        damage *= 2;
    }
    
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
    
    // 检查怪物是否死亡
    if (monster_.isDead()) {
        result.playerWon = true;
        result.expGained = monster_.getExpReward();
        
        oss.str("");
        oss << "\r\n*** VICTORY! ***\r\n";
        oss << "You have defeated the " << monster_.getName() << "!\r\n";
        oss << "You gained " << result.expGained << " experience points!\r\n";
        result.message = oss.str();
        
        endCombat(session, true);
    } else {
        // 怪物反击
        CombatResult monsterResult = monsterAttack(session);
        result.damageReceived = monsterResult.damageDealt;
        result.message += monsterResult.message;
        
        // 检查玩家是否死亡
        if (result.damageReceived >= 100) {  // 假设玩家 100 HP
            result.playerWon = false;
            endCombat(session, false);
        }
    }
    
    return result;
}

CombatResult CombatManager::monsterAttack(Session* session) {
    CombatResult result;
    
    if (state_ != CombatState::InCombat || !monster_.isAlive()) {
        return result;
    }
    
    // 怪物攻击
    int damage = monster_.attack();
    
    // 玩家防御减免（简化）
    int player_defense = 2;
    int actual_damage = std::max(1, damage - player_defense);
    
    result.damageDealt = actual_damage;
    
    std::ostringstream oss;
    oss << "\r\nThe " << monster_.getName() << " attacks you and deals " 
        << actual_damage << " damage!\r\n";
    
    // 玩家 HP 状态
    int player_hp = 100 - actual_damage;  // 简化：不追踪实际 HP
    if (player_hp > 80) {
        oss << "You barely feel the wound.\r\n";
    } else if (player_hp > 60) {
        oss << "You take a minor hit.\r\n";
    } else if (player_hp > 40) {
        oss << "You are starting to feel the pain.\r\n";
    } else if (player_hp > 20) {
        oss << "You are badly hurt! Be careful!\r\n";
    } else {
        oss << "You are on the brink of death! Flee if you can!\r\n";
    }
    
    result.message = oss.str();
    return result;
}

void CombatManager::endCombat(Session* session, bool playerWon) {
    if (playerWon) {
        state_ = CombatState::CombatWon;
        
        // 从房间移除怪物
        auto* room = session->getCurrentRoom();
        if (room) {
            // 这里可以添加移除怪物的逻辑
        }
    } else {
        state_ = CombatState::CombatLost;
        
        std::ostringstream oss;
        oss << "\r\n*** DEFEAT ***\r\n";
        oss << "You have been defeated by the " << monster_.getName() << "...\r\n";
        oss << "You black out and wake up back at the Starting Hall.\r\n";
        session->send(oss.str());
        
        // 传送回起始点
        session->setCurrentRoom("hall");
    }
    
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
