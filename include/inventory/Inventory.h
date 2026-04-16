#ifndef INVENTORY_H
#define INVENTORY_H

#include <string>
#include <vector>
#include "save/SaveManager.h"

namespace mud {

// 背包系统类 - 从Player中分离出来的职责
class Inventory {
public:
    Inventory();
    
    // 添加物品
    void addItem(const Item& item);
    
    // 移除物品
    bool removeItem(const std::string& item_id);
    
    // 检查是否拥有物品
    bool hasItem(const std::string& item_id) const;
    
    // 获取物品
    Item* getItem(const std::string& item_id);
    const Item* getItem(const std::string& item_id) const;
    
    // 获取所有物品
    const std::vector<Item>& getItems() const { return items_; }
    std::vector<Item>& getItems() { return items_; }
    
    // 获取物品数量
    size_t getItemCount() const { return items_.size(); }
    
    // 清空背包
    void clear();
    
    // 获取物品总价值
    int getTotalValue() const;
    
    // 按价值排序
    void sortByValue(bool ascending = false);
    
    // 按名称查找
    std::vector<const Item*> findItemsByName(const std::string& name) const;
    
private:
    std::vector<Item> items_;
};

} // namespace mud

#endif // INVENTORY_H
