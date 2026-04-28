/**
 * @file test_shop.cpp
 * @brief 物品交易 API 的 Mock 单元测试
 * 
 * Prompt 演化实验 2: 物品交易系统测试
 * 覆盖模块：Shop, TradeSystem
 */

#include <gtest/gtest.h>
#include "shop/Shop.h"
#include "player/Player.h"

namespace mud {

// ============================================================================
// Prompt 演化实验 2: 物品交易 API 测试
// ============================================================================

class ShopTest : public ::testing::Test {
protected:
    void SetUp() override {
        player_ = std::make_unique<Player>("TestPlayer");
        player_->setGold(100);  // 初始 100 金币
        shop_ = std::make_unique<Shop>("Test Shop");
    }

    void TearDown() override {
        player_.reset();
        shop_.reset();
    }

    std::unique_ptr<Player> player_;
    std::unique_ptr<Shop> shop_;
};

// ----------------------------------------------------------------------------
// 测试组 1: 商店基础功能
// ----------------------------------------------------------------------------

/**
 * @brief 测试商店默认构造函数
 */
TEST_F(ShopTest, DefaultConstructor) {
    Shop defaultShop;
    EXPECT_EQ(defaultShop.getName(), "Unnamed Shop");
    EXPECT_EQ(defaultShop.getItems().size(), 0);
    EXPECT_FLOAT_EQ(defaultShop.getBuyPriceModifier(), 1.0f);
    EXPECT_FLOAT_EQ(defaultShop.getSellPriceModifier(), 1.0f);
}

/**
 * @brief 测试商店命名构造函数
 */
TEST_F(ShopTest, NamedConstructor) {
    Shop shop("Dragon's Den");
    EXPECT_EQ(shop.getName(), "Dragon's Den");
}

/**
 * @brief 测试添加商品到商店
 */
TEST_F(ShopTest, AddItem) {
    ShopItem item(
        Item("sword_001", "Iron Sword", "A sturdy blade", 50),
        50, 25, 10, true
    );
    
    shop_->addItem(item);
    
    const ShopItem* retrieved = shop_->getItem("sword_001");
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->item.name, "Iron Sword");
    EXPECT_EQ(retrieved->buyPrice, 50);
    EXPECT_EQ(retrieved->sellPrice, 25);
    EXPECT_EQ(retrieved->stock, 10);
    EXPECT_TRUE(retrieved->isSellable);
}

/**
 * @brief 测试移除商品
 */
TEST_F(ShopTest, RemoveItem) {
    shop_->addItem(ShopItem(Item("potion_001", "Potion", "Heals", 25), 25, 12));
    
    EXPECT_TRUE(shop_->removeItem("potion_001"));
    EXPECT_EQ(shop_->getItem("potion_001"), nullptr);
    EXPECT_FALSE(shop_->removeItem("potion_001"));  // 再次移除应失败
}

// ----------------------------------------------------------------------------
// 测试组 2: 购买物品测试 (成功场景)
// ----------------------------------------------------------------------------

/**
 * @brief 测试成功购买物品
 * 
 * Given: 玩家有 100 金币，商店有钢铁之剑 (50 金币)
 * When: 玩家购买钢铁之剑
 * Then: 交易成功，玩家剩余 50 金币，获得物品
 */
TEST_F(ShopTest, BuyItem_Success) {
    // Given
    shop_->addItem(ShopItem(
        Item("sword_001", "Iron Sword", "A sturdy blade", 50),
        50, 25, 10, true
    ));
    
    int initialGold = player_->getGold();
    EXPECT_EQ(initialGold, 100);
    
    // When
    TradeRecord record = shop_->buyItem(*player_, "sword_001", 1);
    
    // Then
    EXPECT_EQ(record.result, TradeResult::Success);
    EXPECT_EQ(record.itemName, "Iron Sword");
    EXPECT_EQ(record.quantity, 1);
    EXPECT_EQ(record.price, 50);
    
    // 验证玩家状态
    EXPECT_EQ(player_->getGold(), 50);  // 100 - 50
    EXPECT_TRUE(player_->hasItem("sword_001"));
    
    // 验证商店库存
    EXPECT_EQ(shop_->getStock("sword_001"), 9);
}

/**
 * @brief 测试购买多个物品
 */
TEST_F(ShopTest, BuyItem_MultipleQuantity) {
    // Given
    shop_->addItem(ShopItem(
        Item("potion_001", "Health Potion", "Restores HP", 25),
        25, 12, 20, true
    ));
    
    // When: 购买 3 瓶药水
    TradeRecord record = shop_->buyItem(*player_, "potion_001", 3);
    
    // Then
    EXPECT_EQ(record.result, TradeResult::Success);
    EXPECT_EQ(record.price, 75);  // 25 * 3
    EXPECT_EQ(player_->getGold(), 25);  // 100 - 75
    
    // 验证获得 3 个物品
    int potionCount = 0;
    for (const auto& item : player_->getInventory()) {
        if (item.id == "potion_001") potionCount++;
    }
    EXPECT_EQ(potionCount, 3);
    
    EXPECT_EQ(shop_->getStock("potion_001"), 17);  // 20 - 3
}

// ----------------------------------------------------------------------------
// 测试组 3: 购买物品测试 (失败场景)
// ----------------------------------------------------------------------------

/**
 * @brief 测试金币不足的情况
 * 
 * Given: 玩家有 30 金币，商店有钢铁之剑 (50 金币)
 * When: 玩家尝试购买
 * Then: 交易失败 (InsufficientGold)
 */
TEST_F(ShopTest, BuyItem_InsufficientGold) {
    // Given
    player_->setGold(30);
    shop_->addItem(ShopItem(
        Item("sword_001", "Iron Sword", "A sturdy blade", 50),
        50, 25, 10, true
    ));
    
    int initialGold = player_->getGold();
    
    // When
    TradeRecord record = shop_->buyItem(*player_, "sword_001", 1);
    
    // Then
    EXPECT_EQ(record.result, TradeResult::InsufficientGold);
    EXPECT_NE(record.message.find("don't have enough gold"), std::string::npos);
    
    // 验证状态未变化
    EXPECT_EQ(player_->getGold(), initialGold);
    EXPECT_FALSE(player_->hasItem("sword_001"));
    EXPECT_EQ(shop_->getStock("sword_001"), 10);
}

/**
 * @brief 测试物品不存在的情况
 */
TEST_F(ShopTest, BuyItem_ItemNotFound) {
    // When
    TradeRecord record = shop_->buyItem(*player_, "nonexistent_item", 1);
    
    // Then
    EXPECT_EQ(record.result, TradeResult::ItemNotFound);
    EXPECT_NE(record.message.find("not found"), std::string::npos);
}

/**
 * @brief 测试无效数量 (零或负数)
 */
TEST_F(ShopTest, BuyItem_InvalidQuantity) {
    shop_->addItem(ShopItem(Item("sword_001", "Sword", "", 50), 50, 25));
    
    // 零数量
    TradeRecord record1 = shop_->buyItem(*player_, "sword_001", 0);
    EXPECT_EQ(record1.result, TradeResult::InvalidQuantity);
    
    // 负数量
    TradeRecord record2 = shop_->buyItem(*player_, "sword_001", -1);
    EXPECT_EQ(record2.result, TradeResult::InvalidQuantity);
}

/**
 * @brief 测试库存不足的情况
 */
TEST_F(ShopTest, BuyItem_OutOfStock) {
    // Given: 商店只有 2 个商品
    shop_->addItem(ShopItem(
        Item("rare_item", "Rare Item", "Very rare", 100),
        100, 50, 2, true
    ));
    
    // When: 尝试购买 5 个
    TradeRecord record = shop_->buyItem(*player_, "rare_item", 5);
    
    // Then
    EXPECT_EQ(record.result, TradeResult::InventoryFull);
    EXPECT_NE(record.message.find("Not enough stock"), std::string::npos);
}

// ----------------------------------------------------------------------------
// 测试组 4: 出售物品测试
// ----------------------------------------------------------------------------

/**
 * @brief 测试成功出售物品
 * 
 * Given: 玩家有钢铁之剑，商店接受此物品
 * When: 玩家出售钢铁之剑
 * Then: 交易成功，玩家获得金币
 */
TEST_F(ShopTest, SellItem_Success) {
    // Given: 玩家先购买物品
    shop_->addItem(ShopItem(
        Item("sword_001", "Iron Sword", "A sturdy blade", 50),
        50, 25, 10, true
    ));
    
    shop_->buyItem(*player_, "sword_001", 1);
    EXPECT_TRUE(player_->hasItem("sword_001"));
    
    int goldAfterBuy = player_->getGold();
    EXPECT_EQ(goldAfterBuy, 50);
    
    // When: 出售物品
    TradeRecord record = shop_->sellItem(*player_, "sword_001", 1);
    
    // Then
    EXPECT_EQ(record.result, TradeResult::Success);
    EXPECT_EQ(record.itemName, "Iron Sword");
    EXPECT_EQ(record.price, 25);  // 卖出价
    
    // 验证玩家状态
    EXPECT_EQ(player_->getGold(), 75);  // 50 + 25
    EXPECT_FALSE(player_->hasItem("sword_001"));
}

/**
 * @brief 测试出售物品数量不足
 */
TEST_F(ShopTest, SellItem_NotEnoughItems) {
    // Given: 玩家只有 1 个物品
    shop_->addItem(ShopItem(Item("potion_001", "Potion", "", 25), 25, 12));
    shop_->buyItem(*player_, "potion_001", 1);
    
    // When: 尝试出售 2 个
    TradeRecord record = shop_->sellItem(*player_, "potion_001", 2);
    
    // Then
    EXPECT_EQ(record.result, TradeResult::ItemNotFound);
    EXPECT_NE(record.message.find("don't have enough"), std::string::npos);
}

/**
 * @brief 测试出售不可接受的物品
 */
TEST_F(ShopTest, SellItem_ItemNotSellable) {
    // Given: 商店有不接受出售的物品
    shop_->addItem(ShopItem(
        Item("quest_item", "Quest Item", "Cannot be sold", 0),
        100, 0, -1, false  // isSellable = false
    ));
    
    // 玩家通过其他方式获得此物品
    player_->addItem(Item("quest_item", "Quest Item", "", 0));
    
    // When: 尝试出售
    TradeRecord record = shop_->sellItem(*player_, "quest_item", 1);
    
    // Then
    EXPECT_EQ(record.result, TradeResult::ItemNotSellable);
    EXPECT_NE(record.message.find("doesn't accept"), std::string::npos);
}

/**
 * @brief 测试出售商店未列出的物品 (按基础价值 50% 回收)
 */
TEST_F(ShopTest, SellItem_UnlistedItem) {
    // Given: 玩家有商店未列出的物品
    Item unlistedItem("treasure", "Ancient Treasure", "Valuable artifact", 200);
    player_->addItem(unlistedItem);
    
    // When: 出售给商店
    TradeRecord record = shop_->sellItem(*player_, "treasure", 1);
    
    // Then: 按基础价值的 50% 计算
    EXPECT_EQ(record.result, TradeResult::Success);
    EXPECT_EQ(record.price, 100);  // 200 * 0.5
    
    EXPECT_FALSE(player_->hasItem("treasure"));
    EXPECT_EQ(player_->getGold(), 200);  // 100 + 100
}

// ----------------------------------------------------------------------------
// 测试组 5: 价格修改器测试
// ----------------------------------------------------------------------------

/**
 * @brief 测试购买价格折扣
 * 
 * Given: 商店购买价格修改器为 0.8(8 折)
 * When: 玩家购买 50 金币的物品
 * Then: 实际支付 40 金币
 */
TEST_F(ShopTest, PriceModifier_BuyDiscount) {
    // Given
    shop_->addItem(ShopItem(
        Item("sword_001", "Iron Sword", "", 50),
        50, 25, 10, true
    ));
    shop_->setBuyPriceModifier(0.8f);  // 8 折
    
    // When
    TradeRecord record = shop_->buyItem(*player_, "sword_001", 1);
    
    // Then
    EXPECT_EQ(record.result, TradeResult::Success);
    EXPECT_EQ(record.price, 40);  // 50 * 0.8
    EXPECT_EQ(player_->getGold(), 60);  // 100 - 40
}

/**
 * @brief 测试出售价格加成
 * 
 * Given: 商店出售价格修改器为 1.2(120%)
 * When: 玩家出售物品
 * Then: 获得 120% 的售价
 */
TEST_F(ShopTest, PriceModifier_SellMarkup) {
    // Given
    shop_->addItem(ShopItem(
        Item("sword_001", "Iron Sword", "", 50),
        50, 25, 10, true
    ));
    shop_->setSellPriceModifier(1.2f);  // 120%
    
    // 玩家先购买
    shop_->buyItem(*player_, "sword_001", 1);
    
    // When: 出售
    TradeRecord record = shop_->sellItem(*player_, "sword_001", 1);
    
    // Then
    EXPECT_EQ(record.result, TradeResult::Success);
    EXPECT_EQ(record.price, 30);  // 25 * 1.2
}

/**
 * @brief 测试价格修改器边界值
 */
TEST_F(ShopTest, PriceModifier_BoundaryValues) {
    shop_->addItem(ShopItem(Item("item", "Item", "", 100), 100, 50));
    
    // 免费 (0 折)
    shop_->setBuyPriceModifier(0.0f);
    TradeRecord r1 = shop_->buyItem(*player_, "item", 1);
    EXPECT_EQ(r1.price, 0);
    shop_->removeItem("item");
    shop_->addItem(ShopItem(Item("item", "Item", "", 100), 100, 50));
    
    // 双倍价格
    shop_->setBuyPriceModifier(2.0f);
    TradeRecord r2 = shop_->buyItem(*player_, "item", 1);
    EXPECT_EQ(r2.price, 200);
}

// ----------------------------------------------------------------------------
// 测试组 6: 库存管理测试
// ----------------------------------------------------------------------------

/**
 * @brief 测试补货功能
 */
TEST_F(ShopTest, StockManagement_Restock) {
    // Given
    shop_->addItem(ShopItem(
        Item("potion_001", "Potion", "", 25),
        25, 12, 5, true
    ));
    EXPECT_EQ(shop_->getStock("potion_001"), 5);
    
    // When: 补货 10 个
    shop_->restock("potion_001", 10);
    
    // Then
    EXPECT_EQ(shop_->getStock("potion_001"), 15);
}

/**
 * @brief 测试无限库存商品
 */
TEST_F(ShopTest, InfiniteStockItem) {
    // Given: 无限库存 (stock = -1)
    shop_->addItem(ShopItem(
        Item("basic_item", "Basic Item", "", 10),
        10, 5, -1, true
    ));
    
    // When: 购买多个
    shop_->buyItem(*player_, "basic_item", 5);
    shop_->buyItem(*player_, "basic_item", 10);
    
    // Then: 库存仍为 -1 (无限)
    EXPECT_EQ(shop_->getStock("basic_item"), -1);
}

/**
 * @brief 测试购买完所有库存
 */
TEST_F(ShopTest, BuyAllStock) {
    // Given
    shop_->addItem(ShopItem(
        Item("limited_item", "Limited Item", "", 10),
        10, 5, 3, true
    ));
    
    // When: 买光所有库存
    player_->setGold(1000);  // 足够金币
    shop_->buyItem(*player_, "limited_item", 3);
    
    // Then
    EXPECT_EQ(shop_->getStock("limited_item"), 0);
    
    // 再尝试购买应失败
    TradeRecord record = shop_->buyItem(*player_, "limited_item", 1);
    EXPECT_EQ(record.result, TradeResult::InventoryFull);
}

// ----------------------------------------------------------------------------
// 测试组 7: 批量交易测试
// ----------------------------------------------------------------------------

/**
 * @brief 测试批量购买
 */
TEST_F(ShopTest, BulkTransaction_Buy) {
    // Given
    shop_->addItem(ShopItem(
        Item("arrow_001", "Arrow", "Ammunition", 5),
        5, 2, 100, true
    ));
    
    // When: 批量购买 20 个
    TradeRecord record = shop_->buyItem(*player_, "arrow_001", 20);
    
    // Then
    EXPECT_EQ(record.result, TradeResult::Success);
    EXPECT_EQ(record.price, 100);  // 5 * 20
    EXPECT_EQ(player_->getGold(), 0);  // 100 - 100
    
    // 验证获得 20 个箭
    int arrowCount = 0;
    for (const auto& item : player_->getInventory()) {
        if (item.id == "arrow_001") arrowCount++;
    }
    EXPECT_EQ(arrowCount, 20);
}

/**
 * @brief 测试批量出售
 */
TEST_F(ShopTest, BulkTransaction_Sell) {
    // Given: 玩家有 10 个药水
    shop_->addItem(ShopItem(
        Item("potion_001", "Potion", "", 25),
        25, 12, 50, true
    ));
    
    // 玩家通过购买获得 10 个药水
    player_->setGold(500);
    for (int i = 0; i < 10; ++i) {
        shop_->buyItem(*player_, "potion_001", 1);
    }
    
    // When: 批量出售 5 个
    TradeRecord record = shop_->sellItem(*player_, "potion_001", 5);
    
    // Then
    EXPECT_EQ(record.result, TradeResult::Success);
    EXPECT_EQ(record.price, 60);  // 12 * 5
    
    // 验证剩余 5 个药水
    int potionCount = 0;
    for (const auto& item : player_->getInventory()) {
        if (item.id == "potion_001") potionCount++;
    }
    EXPECT_EQ(potionCount, 5);
}

// ----------------------------------------------------------------------------
// 测试组 8: 默认商店工厂测试
// ----------------------------------------------------------------------------

/**
 * @brief 测试通用商店工厂
 */
TEST_F(ShopTest, DefaultShopFactory_GeneralStore) {
    Shop shop = DefaultShopFactory::createGeneralStore();
    
    EXPECT_EQ(shop.getName(), "General Store");
    EXPECT_GE(shop.getItems().size(), 3);
    
    // 验证有基础商品
    EXPECT_NE(shop.getItem("sword_iron"), nullptr);
    EXPECT_NE(shop.getItem("shield_wood"), nullptr);
    EXPECT_NE(shop.getItem("potion_hp_small"), nullptr);
}

/**
 * @brief 测试武器商店工厂
 */
TEST_F(ShopTest, DefaultShopFactory_WeaponShop) {
    Shop shop = DefaultShopFactory::createWeaponShop();
    
    EXPECT_EQ(shop.getName(), "Weapon Shop");
    
    // 验证有武器商品
    EXPECT_NE(shop.getItem("sword_steel"), nullptr);
    EXPECT_NE(shop.getItem("axe_battle"), nullptr);
    EXPECT_NE(shop.getItem("dagger"), nullptr);
}

/**
 * @brief 测试药水商店工厂
 */
TEST_F(ShopTest, DefaultShopFactory_PotionShop) {
    Shop shop = DefaultShopFactory::createPotionShop();
    
    EXPECT_EQ(shop.getName(), "Potion Shop");
    
    // 验证有药水商品
    EXPECT_NE(shop.getItem("potion_hp_small"), nullptr);
    EXPECT_NE(shop.getItem("potion_hp_large"), nullptr);
    EXPECT_NE(shop.getItem("elixir"), nullptr);
}

// ----------------------------------------------------------------------------
// 测试组 9: 交易记录消息测试
// ----------------------------------------------------------------------------

/**
 * @brief 测试购买消息格式
 */
TEST_F(ShopTest, BuyMessageFormat) {
    shop_->addItem(ShopItem(
        Item("sword_001", "Iron Sword", "", 50),
        50, 25, 10, true
    ));
    
    TradeRecord record = shop_->buyItem(*player_, "sword_001", 1);
    
    std::string message = shop_->getBuyMessage(record);
    
    EXPECT_NE(message.find("bought"), std::string::npos);
    EXPECT_NE(message.find("Iron Sword"), std::string::npos);
    EXPECT_NE(message.find("50"), std::string::npos);
}

/**
 * @brief 测试出售消息格式
 */
TEST_F(ShopTest, SellMessageFormat) {
    shop_->addItem(ShopItem(
        Item("sword_001", "Iron Sword", "", 50),
        50, 25, 10, true
    ));
    
    player_->addItem(Item("sword_001", "Iron Sword", "", 50));
    TradeRecord record = shop_->sellItem(*player_, "sword_001", 1);
    
    std::string message = shop_->getSellMessage(record);
    
    EXPECT_NE(message.find("sold"), std::string::npos);
    EXPECT_NE(message.find("Iron Sword"), std::string::npos);
}

} // namespace mud
