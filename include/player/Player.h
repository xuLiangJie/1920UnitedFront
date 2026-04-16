#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include "save/SaveManager.h"

namespace mud {

class Player {
public:
    Player();
    explicit Player(const std::string& name);
    
    // 基本信息
    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }
    
    const std::string& getDescription() const { return description_; }
    void setDescription(const std::string& desc) { description_ = desc; }
    
    const std::string& getLocation() const { return location_; }
    void setLocation(const std::string& loc) { location_ = loc; }
    
    bool isLoggedIn() const { return logged_in_; }
    void setLoggedIn(bool logged_in) { logged_in_ = logged_in; }
    
    // 战斗属性
    int getLevel() const { return level_; }
    void setLevel(int level) { level_ = level; }
    
    int getExp() const { return exp_; }
    void setExp(int exp) { exp_ = exp; }
    void addExp(int amount);
    
    int getMaxHP() const { return max_hp_; }
    void setMaxHP(int hp) { max_hp_ = hp; }
    
    int getCurrentHP() const { return current_hp_; }
    void setCurrentHP(int hp) { current_hp_ = hp; }
    void heal(int amount);
    void takeDamage(int amount);
    
    int getMaxMP() const { return max_mp_; }
    void setMaxMP(int mp) { max_mp_ = mp; }
    
    int getCurrentMP() const { return current_mp_; }
    void setCurrentMP(int mp) { current_mp_ = mp; }
    
    int getAttack() const { return attack_; }
    void setAttack(int attack) { attack_ = attack; }
    
    int getDefense() const { return defense_; }
    void setDefense(int defense) { defense_ = defense; }
    
    int getGold() const { return gold_; }
    void setGold(int gold) { gold_ = gold; }
    void addGold(int amount) { gold_ += amount; }
    
    // 背包系统
    const std::vector<Item>& getInventory() const { return inventory_; }
    std::vector<Item>& getInventory() { return inventory_; }
    void addItem(const Item& item);
    bool removeItem(const std::string& item_id);
    bool hasItem(const std::string& item_id) const;
    Item* getItem(const std::string& item_id);
    
    // 存档相关
    PlayerData toData() const;
    void fromData(const PlayerData& data);
    
    std::string getInfo() const;
    std::string getStatus() const;
    
private:
    std::string name_;
    std::string description_;
    std::string location_;
    bool logged_in_;
    
    // 战斗属性
    int level_;
    int exp_;
    int max_hp_;
    int current_hp_;
    int max_mp_;
    int current_mp_;
    int attack_;
    int defense_;
    int gold_;
    
    // 背包
    std::vector<Item> inventory_;
};

} // namespace mud

#endif // PLAYER_H
