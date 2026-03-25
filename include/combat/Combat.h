#ifndef COMBAT_H
#define COMBAT_H

#include "combat/Monster.h"
#include <string>
#include <vector>

namespace mud {

class Session;

// 战斗状态
enum class CombatState {
    NotInCombat,
    InCombat,
    CombatWon,
    CombatLost
};

// 战斗管理器
class CombatManager {
public:
    CombatManager();
    
    // 开始战斗
    void startCombat(Session* session, Monster monster);
    
    // 玩家攻击
    CombatResult playerAttack(Session* session);
    
    // 怪物攻击
    CombatResult monsterAttack(Session* session);
    
    // 结束战斗
    void endCombat(Session* session, bool playerWon);
    
    // 检查战斗状态
    bool isInCombat() const { return state_ == CombatState::InCombat; }
    CombatState getState() const { return state_; }
    const Monster* getMonster() const { return &monster_; }
    Monster* getMonster() { return &monster_; }
    
    // 获取战斗描述
    std::string getCombatStatus() const;
    
private:
    CombatState state_;
    Monster monster_;
};

} // namespace mud

#endif // COMBAT_H
