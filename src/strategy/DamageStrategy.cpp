#include "strategy/DamageStrategy.h"
#include <cstdlib>
#include <algorithm>

namespace mud {

// 物理伤害策略实现
int PhysicalDamageStrategy::calculateDamage(int attack, int defense, bool& isCrit) {
    // 基础伤害波动 +/- 20%
    int variance = attack / 5;
    int min_dmg = attack - variance;
    int max_dmg = attack + variance;
    int base_damage = min_dmg + rand() % (max_dmg - min_dmg + 1);
    
    // 暴击判定 (10%)
    isCrit = (rand() % 100) < 10;
    if (isCrit) {
        base_damage *= 2;
    }
    
    // 防御减免
    int actual_damage = std::max(1, base_damage - defense / 2);
    return actual_damage;
}

std::string PhysicalDamageStrategy::getStrategyName() const {
    return "Physical";
}

// 魔法伤害策略实现
int MagicDamageStrategy::calculateDamage(int attack, int defense, bool& isCrit) {
    // 魔法伤害波动更大 +/- 30%
    int variance = attack * 3 / 10;
    int min_dmg = attack - variance;
    int max_dmg = attack + variance;
    int base_damage = min_dmg + rand() % (max_dmg - min_dmg + 1);
    
    // 魔法暴击率更高 (15%)
    isCrit = (rand() % 100) < 15;
    if (isCrit) {
        base_damage *= 2;
    }
    
    // 魔法伤害无视部分防御
    int actual_damage = std::max(1, base_damage - defense / 4);
    return actual_damage;
}

std::string MagicDamageStrategy::getStrategyName() const {
    return "Magic";
}

// 元素伤害策略实现
int ElementalDamageStrategy::calculateDamage(int attack, int defense, bool& isCrit) {
    // 元素伤害波动 +/- 25%
    int variance = attack / 4;
    int min_dmg = attack - variance;
    int max_dmg = attack + variance;
    int base_damage = min_dmg + rand() % (max_dmg - min_dmg + 1);
    
    // 元素暴击率 (12%)
    isCrit = (rand() % 100) < 12;
    if (isCrit) {
        base_damage *= 2;
    }
    
    // 元素伤害无视一半防御
    int actual_damage = std::max(1, base_damage - defense / 2);
    return actual_damage;
}

std::string ElementalDamageStrategy::getStrategyName() const {
    return "Elemental";
}

} // namespace mud
