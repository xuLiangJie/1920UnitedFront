#include <gtest/gtest.h>
#include "save/SaveManager.h"
#include <filesystem>

namespace mud {

class SaveManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        save_manager_ = &SaveManager::getInstance();
        test_dir_ = "test_saves";
        save_manager_->setSaveDirectory(test_dir_);
    }

    void TearDown() override {
        // 清理测试目录
        if (std::filesystem::exists(test_dir_)) {
            std::filesystem::remove_all(test_dir_);
        }
    }

    SaveManager* save_manager_;
    std::string test_dir_;
};

// 测试单例模式
TEST_F(SaveManagerTest, Singleton) {
    SaveManager& instance1 = SaveManager::getInstance();
    SaveManager& instance2 = SaveManager::getInstance();
    
    EXPECT_EQ(&instance1, &instance2);
}

// 测试文件名生成
TEST_F(SaveManagerTest, GenerateFilename) {
    std::string filename = SaveManager::generateFilename("Arthur");
    EXPECT_EQ(filename, "Arthur.sav");
}

// 测试文件名生成（特殊字符）
TEST_F(SaveManagerTest, GenerateFilenameSpecialChars) {
    std::string filename = SaveManager::generateFilename("Ar thur@#$%");
    EXPECT_EQ(filename, "Ar_thur_____.sav");
}

// 测试保存玩家
TEST_F(SaveManagerTest, SavePlayer) {
    PlayerData data;
    data.name = "TestPlayer";
    data.level = 5;
    data.exp = 250;
    data.gold = 100;
    
    std::string filename = SaveManager::generateFilename("TestPlayer");
    bool result = save_manager_->savePlayer(filename, data);
    
    EXPECT_TRUE(result);
    EXPECT_TRUE(save_manager_->hasSave(filename));
}

// 测试加载玩家
TEST_F(SaveManagerTest, LoadPlayer) {
    PlayerData original_data;
    original_data.name = "LoadTest";
    original_data.level = 10;
    original_data.exp = 500;
    original_data.max_hp = 200;
    original_data.current_hp = 150;
    original_data.max_mp = 100;
    original_data.current_mp = 80;
    original_data.attack = 25;
    original_data.defense = 15;
    original_data.gold = 500;
    
    std::string filename = SaveManager::generateFilename("LoadTest");
    save_manager_->savePlayer(filename, original_data);
    
    PlayerData loaded_data;
    bool result = save_manager_->loadPlayer(filename, loaded_data);
    
    EXPECT_TRUE(result);
    EXPECT_EQ(loaded_data.name, original_data.name);
    EXPECT_EQ(loaded_data.level, original_data.level);
    EXPECT_EQ(loaded_data.exp, original_data.exp);
    EXPECT_EQ(loaded_data.gold, original_data.gold);
}

// 测试加载不存在的存档
TEST_F(SaveManagerTest, LoadNonexistentSave) {
    std::string filename = SaveManager::generateFilename("Nonexistent");
    
    EXPECT_FALSE(save_manager_->hasSave(filename));
    
    PlayerData data;
    bool result = save_manager_->loadPlayer(filename, data);
    EXPECT_FALSE(result);
}

// 测试删除存档
TEST_F(SaveManagerTest, DeleteSave) {
    PlayerData data;
    data.name = "DeleteTest";
    
    std::string filename = SaveManager::generateFilename("DeleteTest");
    save_manager_->savePlayer(filename, data);
    
    EXPECT_TRUE(save_manager_->hasSave(filename));
    
    bool result = save_manager_->deleteSave(filename);
    EXPECT_TRUE(result);
    EXPECT_FALSE(save_manager_->hasSave(filename));
}

// 测试删除不存在的存档
TEST_F(SaveManagerTest, DeleteNonexistentSave) {
    std::string filename = SaveManager::generateFilename("Nonexistent");
    
    bool result = save_manager_->deleteSave(filename);
    EXPECT_FALSE(result);
}

// 测试获取存档列表
TEST_F(SaveManagerTest, GetSaveList) {
    // 创建几个测试存档
    PlayerData data1, data2, data3;
    data1.name = "Player1";
    data2.name = "Player2";
    data3.name = "Player3";
    
    save_manager_->savePlayer(SaveManager::generateFilename("Player1"), data1);
    save_manager_->savePlayer(SaveManager::generateFilename("Player2"), data2);
    save_manager_->savePlayer(SaveManager::generateFilename("Player3"), data3);
    
    auto saves = save_manager_->getSaveList();
    
    EXPECT_EQ(saves.size(), 3);
}

// 测试空存档列表
TEST_F(SaveManagerTest, EmptySaveList) {
    auto saves = save_manager_->getSaveList();
    EXPECT_TRUE(saves.empty());
}

// 测试设置存档目录
TEST_F(SaveManagerTest, SetSaveDirectory) {
    std::string new_dir = "custom_saves";
    save_manager_->setSaveDirectory(new_dir);
    
    EXPECT_EQ(save_manager_->getSaveDirectory(), new_dir);
    
    // 清理
    if (std::filesystem::exists(new_dir)) {
        std::filesystem::remove_all(new_dir);
    }
}

// 测试物品数据序列化
TEST_F(SaveManagerTest, ItemSerialization) {
    PlayerData data;
    data.name = "ItemTest";
    
    Item item1("sword_001", "Iron Sword", "A sword", 50);
    Item item2("shield_001", "Wooden Shield", "A shield", 30);
    
    data.inventory.push_back(item1);
    data.inventory.push_back(item2);
    
    std::string filename = SaveManager::generateFilename("ItemTest");
    save_manager_->savePlayer(filename, data);
    
    PlayerData loaded_data;
    save_manager_->loadPlayer(filename, loaded_data);
    
    EXPECT_EQ(loaded_data.inventory.size(), 2);
    EXPECT_EQ(loaded_data.inventory[0].name, "Iron Sword");
    EXPECT_EQ(loaded_data.inventory[1].name, "Wooden Shield");
}

// 测试完整玩家数据
TEST_F(SaveManagerTest, FullPlayerData) {
    PlayerData original;
    original.name = "FullTest";
    original.description = "A test character";
    original.location = "hall";
    original.level = 15;
    original.exp = 750;
    original.max_hp = 300;
    original.current_hp = 250;
    original.max_mp = 150;
    original.current_mp = 100;
    original.attack = 35;
    original.defense = 20;
    original.gold = 1000;
    
    Item item("ring_001", "Gold Ring", "A valuable ring", 200);
    original.inventory.push_back(item);
    
    std::string filename = SaveManager::generateFilename("FullTest");
    save_manager_->savePlayer(filename, original);
    
    PlayerData loaded;
    save_manager_->loadPlayer(filename, loaded);
    
    EXPECT_EQ(loaded.name, original.name);
    EXPECT_EQ(loaded.level, original.level);
    EXPECT_EQ(loaded.current_hp, original.current_hp);
    EXPECT_EQ(loaded.gold, original.gold);
    EXPECT_EQ(loaded.inventory.size(), original.inventory.size());
}

} // namespace mud
