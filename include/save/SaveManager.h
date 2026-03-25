#ifndef SAVE_MANAGER_H
#define SAVE_MANAGER_H

#include <string>
#include <vector>
#include <unordered_map>

namespace mud {

// 物品结构
struct Item {
    std::string id;
    std::string name;
    std::string description;
    int value;
    
    Item() : value(0) {}
    Item(const std::string& i, const std::string& n, const std::string& d, int v)
        : id(i), name(n), description(d), value(v) {}
};

// 玩家数据结构
struct PlayerData {
    std::string name;
    std::string description;
    std::string location;
    int level;
    int exp;
    int max_hp;
    int current_hp;
    int max_mp;
    int current_mp;
    int attack;
    int defense;
    int gold;
    std::vector<Item> inventory;
    
    PlayerData() 
        : level(1), exp(0), max_hp(100), current_hp(100),
          max_mp(50), current_mp(50), attack(10), defense(5), gold(0) {}
};

// 存档管理器
class SaveManager {
public:
    static SaveManager& getInstance();
    
    // 存档路径
    void setSaveDirectory(const std::string& dir);
    std::string getSaveDirectory() const { return save_dir_; }
    
    // 保存/加载玩家数据
    bool savePlayer(const std::string& filename, const PlayerData& data);
    bool loadPlayer(const std::string& filename, PlayerData& data);
    
    // 检查存档是否存在
    bool hasSave(const std::string& filename) const;
    
    // 删除存档
    bool deleteSave(const std::string& filename);
    
    // 获取所有存档列表
    std::vector<std::string> getSaveList() const;
    
    // 生成存档文件名（基于角色名）
    static std::string generateFilename(const std::string& characterName);
    
private:
    SaveManager();
    ~SaveManager();
    
    // JSON 辅助函数
    std::string toJson(const PlayerData& data) const;
    PlayerData fromJson(const std::string& json) const;
    
    std::string save_dir_;
};

} // namespace mud

#endif // SAVE_MANAGER_H
