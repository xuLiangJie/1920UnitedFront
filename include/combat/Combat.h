#ifndef COMBAT_H
#define COMBAT_H

#include "combat/Monster.h"
#include "event/EventBus.h"
#include <string>
#include <vector>
#include <functional>

namespace mud {

class Session;

// 战斗状态
enum class CombatState {
    NotInCombat,
    InCombat,
    CombatWon,
    CombatLost
};

// 战斗结果
struct CombatResult {
    bool playerWon;
    int damageDealt;
    int damageReceived;
    int expGained;
    std::string message;
    uint32_t sessionId;  // 添加sessionId用于事件处理

    CombatResult() : playerWon(false), damageDealt(0), damageReceived(0), expGained(0), sessionId(0) {}
};

// 战斗管理器 - 通过事件总线解耦
class CombatManager {
public:
    CombatManager();

    // 开始战斗 - 发布事件而非直接发送消息
    void startCombat(uint32_t sessionId, Monster monster);

    // 玩家攻击 - 返回结果,由调用者处理事件
    CombatResult playerAttack(uint32_t sessionId, int playerAttack, int playerDefense, int playerHP);

    // 怪物攻击 - 返回结果,由调用者处理事件
    CombatResult monsterAttack(uint32_t sessionId, int playerDefense, int playerHP);

    // 结束战斗 - 发布事件
    void endCombat(uint32_t sessionId, bool playerWon);

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
