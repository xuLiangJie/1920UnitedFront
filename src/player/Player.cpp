#include "player/Player.h"
#include <sstream>
#include <algorithm>

namespace mud {

Player::Player()
    : name_("Stranger")
    , description_("A mysterious adventurer")
    , location_("hall")
    , logged_in_(false)
    , level_(1)
    , exp_(0)
    , max_hp_(100)
    , current_hp_(100)
    , max_mp_(50)
    , current_mp_(50)
    , attack_(10)
    , defense_(5)
    , gold_(0) {
}

Player::Player(const std::string& name)
    : name_(name)
    , description_("A mysterious adventurer")
    , location_("hall")
    , logged_in_(false)
    , level_(1)
    , exp_(0)
    , max_hp_(100)
    , current_hp_(100)
    , max_mp_(50)
    , current_mp_(50)
    , attack_(10)
    , defense_(5)
    , gold_(0) {
}

void Player::addExp(int amount) {
    exp_ += amount;
    
    // 升级系统：每 100 EXP 升一级
    int expNeeded = level_ * 100;
    while (exp_ >= expNeeded) {
        level_++;
        exp_ -= expNeeded;
        max_hp_ += 20;
        current_hp_ = max_hp_;
        max_mp_ += 10;
        current_mp_ = max_mp_;
        attack_ += 2;
        defense_ += 1;
        expNeeded = level_ * 100;
    }
}

void Player::heal(int amount) {
    current_hp_ += amount;
    if (current_hp_ > max_hp_) {
        current_hp_ = max_hp_;
    }
}

void Player::takeDamage(int amount) {
    int actual_damage = std::max(1, amount - defense_ / 2);
    current_hp_ -= actual_damage;
    if (current_hp_ < 0) {
        current_hp_ = 0;
    }
}

void Player::addItem(const Item& item) {
    inventory_.push_back(item);
}

bool Player::removeItem(const std::string& item_id) {
    auto it = std::find_if(inventory_.begin(), inventory_.end(),
        [&item_id](const Item& item) { return item.id == item_id; });
    
    if (it != inventory_.end()) {
        inventory_.erase(it);
        return true;
    }
    return false;
}

bool Player::hasItem(const std::string& item_id) const {
    return std::find_if(inventory_.begin(), inventory_.end(),
        [&item_id](const Item& item) { return item.id == item_id; }) != inventory_.end();
}

Item* Player::getItem(const std::string& item_id) {
    auto it = std::find_if(inventory_.begin(), inventory_.end(),
        [&item_id](const Item& item) { return item.id == item_id; });
    
    if (it != inventory_.end()) {
        return &(*it);
    }
    return nullptr;
}

PlayerData Player::toData() const {
    PlayerData data;
    data.name = name_;
    data.description = description_;
    data.location = location_;
    data.level = level_;
    data.exp = exp_;
    data.max_hp = max_hp_;
    data.current_hp = current_hp_;
    data.max_mp = max_mp_;
    data.current_mp = current_mp_;
    data.attack = attack_;
    data.defense = defense_;
    data.gold = gold_;
    data.inventory = inventory_;
    return data;
}

void Player::fromData(const PlayerData& data) {
    name_ = data.name;
    description_ = data.description;
    location_ = data.location;
    level_ = data.level;
    exp_ = data.exp;
    max_hp_ = data.max_hp;
    current_hp_ = data.current_hp;
    max_mp_ = data.max_mp;
    current_mp_ = data.current_mp;
    attack_ = data.attack;
    defense_ = data.defense;
    gold_ = data.gold;
    inventory_ = data.inventory;
    logged_in_ = true;
}

std::string Player::getInfo() const {
    std::ostringstream oss;
    oss << name_ << " - " << description_;
    return oss.str();
}

std::string Player::getStatus() const {
    std::ostringstream oss;
    oss << "Lv." << level_ << " " << name_ 
        << " (HP: " << current_hp_ << "/" << max_hp_ 
        << ", MP: " << current_mp_ << "/" << max_mp_ << ")";
    return oss.str();
}

} // namespace mud
