#include <gtest/gtest.h>
#include "inventory/Inventory.h"
#include "save/SaveManager.h"

namespace mud {

class InventoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        inventory_ = std::make_unique<Inventory>();
    }

    void TearDown() override {
        inventory_.reset();
    }

    std::unique_ptr<Inventory> inventory_;
};

// 测试默认构造函数
TEST_F(InventoryTest, DefaultConstructor) {
    EXPECT_EQ(inventory_->getItemCount(), 0);
    EXPECT_EQ(inventory_->getTotalValue(), 0);
    EXPECT_TRUE(inventory_->getItems().empty());
}

// 测试添加物品
TEST_F(InventoryTest, AddItem) {
    Item item("sword_001", "Iron Sword", "A sturdy blade", 50);
    inventory_->addItem(item);
    
    EXPECT_EQ(inventory_->getItemCount(), 1);
    EXPECT_TRUE(inventory_->hasItem("sword_001"));
}

// 测试添加多个物品
TEST_F(InventoryTest, AddMultipleItems) {
    inventory_->addItem({"sword_001", "Iron Sword", "A sword", 50});
    inventory_->addItem({"shield_001", "Wooden Shield", "A shield", 30});
    inventory_->addItem({"potion_001", "Health Potion", "Heals HP", 25});
    
    EXPECT_EQ(inventory_->getItemCount(), 3);
    EXPECT_TRUE(inventory_->hasItem("sword_001"));
    EXPECT_TRUE(inventory_->hasItem("shield_001"));
    EXPECT_TRUE(inventory_->hasItem("potion_001"));
}

// 测试移除物品
TEST_F(InventoryTest, RemoveItem) {
    inventory_->addItem({"sword_001", "Iron Sword", "A sword", 50});
    
    EXPECT_TRUE(inventory_->hasItem("sword_001"));
    EXPECT_TRUE(inventory_->removeItem("sword_001"));
    EXPECT_FALSE(inventory_->hasItem("sword_001"));
    EXPECT_EQ(inventory_->getItemCount(), 0);
}

// 测试移除不存在的物品
TEST_F(InventoryTest, RemoveNonexistentItem) {
    EXPECT_FALSE(inventory_->removeItem("nonexistent"));
}

// 测试获取物品
TEST_F(InventoryTest, GetItem) {
    Item item("sword_001", "Iron Sword", "A sword", 50);
    inventory_->addItem(item);
    
    Item* retrieved = inventory_->getItem("sword_001");
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->name, "Iron Sword");
    EXPECT_EQ(retrieved->value, 50);
}

// 测试获取不存在的物品
TEST_F(InventoryTest, GetNonexistentItem) {
    const Item* item = inventory_->getItem("nonexistent");
    EXPECT_EQ(item, nullptr);
    
    Item* mutable_item = inventory_->getItem("nonexistent");
    EXPECT_EQ(mutable_item, nullptr);
}

// 测试清空背包
TEST_F(InventoryTest, Clear) {
    inventory_->addItem({"item1", "Item 1", "First", 10});
    inventory_->addItem({"item2", "Item 2", "Second", 20});
    
    EXPECT_EQ(inventory_->getItemCount(), 2);
    inventory_->clear();
    EXPECT_EQ(inventory_->getItemCount(), 0);
}

// 测试总价值计算
TEST_F(InventoryTest, TotalValue) {
    inventory_->addItem({"item1", "Item 1", "First", 10});
    inventory_->addItem({"item2", "Item 2", "Second", 20});
    inventory_->addItem({"item3", "Item 3", "Third", 30});
    
    EXPECT_EQ(inventory_->getTotalValue(), 60);
}

// 测试空背包总价值
TEST_F(InventoryTest, EmptyTotalValue) {
    EXPECT_EQ(inventory_->getTotalValue(), 0);
}

// 测试按价值排序（降序）
TEST_F(InventoryTest, SortByValueDescending) {
    inventory_->addItem({"item1", "Item 1", "First", 10});
    inventory_->addItem({"item2", "Item 2", "Second", 30});
    inventory_->addItem({"item3", "Item 3", "Third", 20});
    
    inventory_->sortByValue(false); // 降序
    
    const auto& items = inventory_->getItems();
    EXPECT_GE(items[0].value, items[1].value);
    EXPECT_GE(items[1].value, items[2].value);
}

// 测试按价值排序（升序）
TEST_F(InventoryTest, SortByValueAscending) {
    inventory_->addItem({"item1", "Item 1", "First", 30});
    inventory_->addItem({"item2", "Item 2", "Second", 10});
    inventory_->addItem({"item3", "Item 3", "Third", 20});
    
    inventory_->sortByValue(true); // 升序
    
    const auto& items = inventory_->getItems();
    EXPECT_LE(items[0].value, items[1].value);
    EXPECT_LE(items[1].value, items[2].value);
}

// 测试按名称查找
TEST_F(InventoryTest, FindItemsByName) {
    inventory_->addItem({"sword_001", "Iron Sword", "Iron", 50});
    inventory_->addItem({"sword_002", "Steel Sword", "Steel", 100});
    inventory_->addItem({"shield_001", "Wooden Shield", "Wood", 30});
    
    auto swords = inventory_->findItemsByName("Sword");
    EXPECT_EQ(swords.size(), 2);
    
    auto shields = inventory_->findItemsByName("Shield");
    EXPECT_EQ(shields.size(), 1);
    
    auto nonexistent = inventory_->findItemsByName("Bow");
    EXPECT_EQ(nonexistent.size(), 0);
}

// 测试部分匹配
TEST_F(InventoryTest, PartialNameMatch) {
    inventory_->addItem({"item_iron", "Iron Item", "Iron", 10});
    inventory_->addItem({"item_steel", "Steel Item", "Steel", 20});
    inventory_->addItem({"item_gold", "Gold Item", "Gold", 30});
    
    auto iron_items = inventory_->findItemsByName("Iron");
    EXPECT_EQ(iron_items.size(), 1);
    
    auto item_items = inventory_->findItemsByName("Item");
    EXPECT_EQ(item_items.size(), 3);
}

} // namespace mud
