#include "combat/Monster.h"
#include <sstream>
#include <algorithm>

namespace mud {

Monster::Monster() 
    : max_hp_(10), current_hp_(10), attack_(5), defense_(0), exp_reward_(10)
    , level_(MonsterLevel::Easy) {
}

Monster::Monster(const std::string& name, const std::string& desc, MonsterLevel level)
    : name_(name), description_(desc), level_(level) {
    initStats();
}

void Monster::initStats() {
    switch (level_) {
        case MonsterLevel::Easy:
            max_hp_ = 20 + rand() % 10;
            current_hp_ = max_hp_;
            attack_ = 3 + rand() % 3;
            defense_ = 0;
            exp_reward_ = 10;
            break;
        case MonsterLevel::Normal:
            max_hp_ = 40 + rand() % 20;
            current_hp_ = max_hp_;
            attack_ = 6 + rand() % 4;
            defense_ = 1;
            exp_reward_ = 25;
            break;
        case MonsterLevel::Hard:
            max_hp_ = 80 + rand() % 30;
            current_hp_ = max_hp_;
            attack_ = 10 + rand() % 5;
            defense_ = 2;
            exp_reward_ = 50;
            break;
        case MonsterLevel::Boss:
            max_hp_ = 150 + rand() % 50;
            current_hp_ = max_hp_;
            attack_ = 15 + rand() % 8;
            defense_ = 4;
            exp_reward_ = 150;
            break;
    }
}

int Monster::calculateDamage(int base_attack) const {
    // 基础伤害波动 +/- 20%
    int variance = base_attack / 5;
    int min_dmg = base_attack - variance;
    int max_dmg = base_attack + variance;
    return min_dmg + rand() % (max_dmg - min_dmg + 1);
}

int Monster::attack() {
    return calculateDamage(attack_);
}

void Monster::takeDamage(int damage) {
    // 防御减免
    int actual_damage = std::max(1, damage - defense_);
    current_hp_ -= actual_damage;
    if (current_hp_ < 0) current_hp_ = 0;
}

void Monster::heal(int amount) {
    current_hp_ += amount;
    if (current_hp_ > max_hp_) current_hp_ = max_hp_;
}

std::string Monster::getHealthStatus() const {
    int percent = (current_hp_ * 100) / max_hp_;
    std::ostringstream oss;
    
    if (percent > 80) {
        oss << name_ << " looks healthy and ready to fight!";
    } else if (percent > 60) {
        oss << name_ << " has some minor wounds.";
    } else if (percent > 40) {
        oss << name_ << " is bleeding and breathing heavily.";
    } else if (percent > 20) {
        oss << name_ << " is severely wounded and staggering!";
    } else {
        oss << name_ << " is on the verge of death!";
    }
    
    return oss.str();
}

std::string Monster::getDeathMessage() const {
    std::ostringstream oss;
    oss << "The " << name_ << " lets out a final cry and collapses to the ground, dead.";
    return oss.str();
}

// ==================== 预设怪物 ====================

Monster Monster::createGoblin() {
    Monster goblin("Goblin", "A small green creature with beady eyes and a rusty dagger", 
                   MonsterLevel::Easy);
    goblin.initStats();
    return goblin;
}

Monster Monster::createWolf() {
    Monster wolf("Wild Wolf", "A fierce wolf with glowing yellow eyes and sharp fangs", 
                 MonsterLevel::Easy);
    wolf.initStats();
    return wolf;
}

Monster Monster::createSkeleton() {
    Monster skeleton("Skeleton Warrior", "An animated skeleton wielding an old sword", 
                     MonsterLevel::Normal);
    skeleton.initStats();
    return skeleton;
}

Monster Monster::createOrc() {
    Monster orc("Orc Berserker", "A muscular orc with a battle axe, covered in scars", 
                MonsterLevel::Hard);
    orc.initStats();
    return orc;
}

Monster Monster::createDragon() {
    Monster dragon("Young Dragon", "A magnificent dragon with shimmering scales and fiery breath", 
                   MonsterLevel::Boss);
    dragon.initStats();
    return dragon;
}

} // namespace mud
