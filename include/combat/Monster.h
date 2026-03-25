#ifndef MONSTER_H
#define MONSTER_H

#include <string>
#include <cstdlib>
#include <ctime>

namespace mud {

// 怪物难度等级
enum class MonsterLevel {
    Easy,      // 简单
    Normal,    // 普通
    Hard,      // 困难
    Boss       // Boss
};

class Monster {
public:
    Monster();
    Monster(const std::string& name, const std::string& desc, MonsterLevel level);
    
    // Getter
    const std::string& getName() const { return name_; }
    const std::string& getDescription() const { return description_; }
    MonsterLevel getLevel() const { return level_; }
    
    int getMaxHP() const { return max_hp_; }
    int getCurrentHP() const { return current_hp_; }
    int getAttack() const { return attack_; }
    int getDefense() const { return defense_; }
    int getExpReward() const { return exp_reward_; }
    
    bool isAlive() const { return current_hp_ > 0; }
    bool isDead() const { return current_hp_ <= 0; }
    
    // 战斗操作
    int attack();  // 怪物攻击，返回伤害值
    void takeDamage(int damage);  // 承受伤害
    void heal(int amount);  // 治疗
    
    // 获取战斗描述
    std::string getHealthStatus() const;
    std::string getDeathMessage() const;
    
    // 静态方法：创建预设怪物
    static Monster createGoblin();
    static Monster createWolf();
    static Monster createSkeleton();
    static Monster createOrc();
    static Monster createDragon();
    
private:
    void initStats();
    int calculateDamage(int base_attack) const;
    
    std::string name_;
    std::string description_;
    MonsterLevel level_;
    
    int max_hp_;
    int current_hp_;
    int attack_;
    int defense_;
    int exp_reward_;
};

// 战斗结果
struct CombatResult {
    bool playerWon;
    int damageDealt;
    int damageReceived;
    int expGained;
    std::string message;
    
    CombatResult() : playerWon(false), damageDealt(0), damageReceived(0), expGained(0) {}
};

} // namespace mud

#endif // MONSTER_H
