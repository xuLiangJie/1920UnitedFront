#ifndef DAMAGE_STRATEGY_H
#define DAMAGE_STRATEGY_H

#include <string>

namespace mud {

class Session;

// 伤害策略接口 - 策略模式
class DamageStrategy {
public:
    virtual ~DamageStrategy() = default;
    
    // 计算伤害
    virtual int calculateDamage(int attack, int defense, bool& isCrit) = 0;
    
    // 获取策略名称
    virtual std::string getStrategyName() const = 0;
};

// 物理伤害策略
class PhysicalDamageStrategy : public DamageStrategy {
public:
    int calculateDamage(int attack, int defense, bool& isCrit) override;
    std::string getStrategyName() const override;
};

// 魔法伤害策略(预留扩展)
class MagicDamageStrategy : public DamageStrategy {
public:
    int calculateDamage(int attack, int defense, bool& isCrit) override;
    std::string getStrategyName() const override;
};

// 元素伤害策略(预留扩展)
class ElementalDamageStrategy : public DamageStrategy {
public:
    int calculateDamage(int attack, int defense, bool& isCrit) override;
    std::string getStrategyName() const override;
};

} // namespace mud

#endif // DAMAGE_STRATEGY_H
