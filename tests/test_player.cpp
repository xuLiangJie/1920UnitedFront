#include <gtest/gtest.h>
#include "player/Player.h"
#include "save/SaveManager.h"

namespace mud {

class PlayerTest : public ::testing::Test {
protected:
    void SetUp() override {
        player_ = std::make_unique<Player>();
    }

    void TearDown() override {
        player_.reset();
    }

    std::unique_ptr<Player> player_;
};

// 测试默认构造函数
TEST_F(PlayerTest, DefaultConstructor) {
    EXPECT_EQ(player_->getName(), "Stranger");
    EXPECT_EQ(player_->getDescription(), "A mysterious adventurer");
    EXPECT_EQ(player_->getLocation(), "hall");
    EXPECT_FALSE(player_->isLoggedIn());
    EXPECT_EQ(player_->getLevel(), 1);
    EXPECT_EQ(player_->getExp(), 0);
    EXPECT_EQ(player_->getMaxHP(), 100);
    EXPECT_EQ(player_->getCurrentHP(), 100);
    EXPECT_EQ(player_->getMaxMP(), 50);
    EXPECT_EQ(player_->getCurrentMP(), 50);
    EXPECT_EQ(player_->getAttack(), 10);
    EXPECT_EQ(player_->getDefense(), 5);
    EXPECT_EQ(player_->getGold(), 0);
}

// 测试命名构造函数
TEST_F(PlayerTest, NamedConstructor) {
    Player player("Arthur");
    EXPECT_EQ(player.getName(), "Arthur");
    EXPECT_FALSE(player.isLoggedIn());
}

// 测试设置名称
TEST_F(PlayerTest, SetName) {
    player_->setName("Lancelot");
    EXPECT_EQ(player_->getName(), "Lancelot");
}

// 测试设置登录状态
TEST_F(PlayerTest, SetLoggedIn) {
    EXPECT_FALSE(player_->isLoggedIn());
    player_->setLoggedIn(true);
    EXPECT_TRUE(player_->isLoggedIn());
}

// 测试添加经验值
TEST_F(PlayerTest, AddExp) {
    player_->addExp(50);
    EXPECT_EQ(player_->getExp(), 50);
    EXPECT_EQ(player_->getLevel(), 1); // 未升级
}

// 测试升级系统
TEST_F(PlayerTest, LevelUp) {
    player_->addExp(100); // 需要100EXP升级
    EXPECT_EQ(player_->getLevel(), 2);
    EXPECT_EQ(player_->getExp(), 0); // EXP重置
    EXPECT_EQ(player_->getMaxHP(), 120); // +20 HP
    EXPECT_EQ(player_->getMaxMP(), 60);  // +10 MP
    EXPECT_EQ(player_->getAttack(), 12); // +2 ATK
    EXPECT_EQ(player_->getDefense(), 6); // +1 DEF
}

// 测试多次升级
TEST_F(PlayerTest, MultipleLevelUp) {
    player_->addExp(300); // 应该升3级 (100+200+300)
    EXPECT_EQ(player_->getLevel(), 3);
}

// 测试治疗
TEST_F(PlayerTest, Heal) {
    player_->takeDamage(50);
    EXPECT_EQ(player_->getCurrentHP(), 51); // 100 - (50 - 5/2)
    player_->heal(30);
    EXPECT_EQ(player_->getCurrentHP(), 81);
}

// 测试治疗不超过最大HP
TEST_F(PlayerTest, HealOverMax) {
    player_->heal(50);
    EXPECT_EQ(player_->getCurrentHP(), 100); // 不应该超过最大值
}

// 测试受到伤害
TEST_F(PlayerTest, TakeDamage) {
    player_->takeDamage(20);
    int expected_damage = std::max(1, 20 - 5 / 2); // 防御减免
    EXPECT_EQ(player_->getCurrentHP(), 100 - expected_damage);
}

// 测试伤害不会低于0
TEST_F(PlayerTest, DamageNotBelowZero) {
    player_->takeDamage(1000);
    EXPECT_EQ(player_->getCurrentHP(), 0);
}

// 测试金币管理
TEST_F(PlayerTest, GoldManagement) {
    EXPECT_EQ(player_->getGold(), 0);
    player_->setGold(100);
    EXPECT_EQ(player_->getGold(), 100);
    player_->addGold(50);
    EXPECT_EQ(player_->getGold(), 150);
    player_->addGold(-30);
    EXPECT_EQ(player_->getGold(), 120);
}

// 测试物品系统
TEST_F(PlayerTest, ItemManagement) {
    Item item("sword_001", "Iron Sword", "A sturdy blade", 50);
    
    EXPECT_FALSE(player_->hasItem("sword_001"));
    player_->addItem(item);
    EXPECT_TRUE(player_->hasItem("sword_001"));
    EXPECT_EQ(player_->getInventory().size(), 1);
    
    // 获取物品
    Item* retrieved = player_->getItem("sword_001");
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->name, "Iron Sword");
    
    // 移除物品
    EXPECT_TRUE(player_->removeItem("sword_001"));
    EXPECT_FALSE(player_->hasItem("sword_001"));
    EXPECT_EQ(player_->getInventory().size(), 0);
    
    // 移除不存在的物品
    EXPECT_FALSE(player_->removeItem("nonexistent"));
}

// 测试数据转换
TEST_F(PlayerTest, DataConversion) {
    player_->setName("Merlin");
    player_->setLevel(5);
    player_->setExp(250);
    player_->setGold(100);
    
    Item item("potion_001", "Health Potion", "Restores HP", 25);
    player_->addItem(item);
    
    PlayerData data = player_->toData();
    
    EXPECT_EQ(data.name, "Merlin");
    EXPECT_EQ(data.level, 5);
    EXPECT_EQ(data.exp, 250);
    EXPECT_EQ(data.gold, 100);
    EXPECT_EQ(data.inventory.size(), 1);
    
    // 从数据加载
    Player newPlayer;
    newPlayer.fromData(data);
    
    EXPECT_EQ(newPlayer.getName(), "Merlin");
    EXPECT_EQ(newPlayer.getLevel(), 5);
    EXPECT_TRUE(newPlayer.isLoggedIn());
}

// 测试获取信息
TEST_F(PlayerTest, GetInfo) {
    player_->setName("Gandalf");
    player_->setDescription("A powerful wizard");
    std::string info = player_->getInfo();
    EXPECT_NE(info.find("Gandalf"), std::string::npos);
    EXPECT_NE(info.find("wizard"), std::string::npos);
}

// 测试获取状态
TEST_F(PlayerTest, GetStatus) {
    player_->setName("Frodo");
    player_->setLevel(3);
    std::string status = player_->getStatus();
    EXPECT_NE(status.find("Frodo"), std::string::npos);
    EXPECT_NE(status.find("Lv.3"), std::string::npos);
}

} // namespace mud
