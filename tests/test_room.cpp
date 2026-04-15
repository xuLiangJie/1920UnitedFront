#include <gtest/gtest.h>
#include "world/Room.h"
#include "combat/Monster.h"

namespace mud {

class RoomTest : public ::testing::Test {
protected:
    void SetUp() override {
        room_ = std::make_unique<Room>("test_room", "Test Room", "A room for testing");
    }

    void TearDown() override {
        room_.reset();
    }

    std::unique_ptr<Room> room_;
};

// 测试默认构造函数
TEST_F(RoomTest, DefaultConstructor) {
    Room empty_room;
    EXPECT_EQ(empty_room.getId(), "");
    EXPECT_EQ(empty_room.getName(), "");
    EXPECT_EQ(empty_room.getDescription(), "");
    EXPECT_TRUE(empty_room.getExits().empty());
    EXPECT_TRUE(empty_room.getNPCs().empty());
    EXPECT_TRUE(empty_room.getMonsters().empty());
}

// 测试命名构造函数
TEST_F(RoomTest, NamedConstructor) {
    EXPECT_EQ(room_->getId(), "test_room");
    EXPECT_EQ(room_->getName(), "Test Room");
    EXPECT_EQ(room_->getDescription(), "A room for testing");
}

// 测试设置描述
TEST_F(RoomTest, SetDescription) {
    room_->setDescription("New description");
    EXPECT_EQ(room_->getDescription(), "New description");
}

// 测试设置出口
TEST_F(RoomTest, SetExit) {
    room_->setExit(Direction::North, "north_room");
    EXPECT_EQ(room_->getExit(Direction::North), "north_room");
    EXPECT_EQ(room_->getExit(Direction::South), ""); // 未设置的出口应该为空
}

// 测试多个出口
TEST_F(RoomTest, MultipleExits) {
    room_->setExit(Direction::North, "north");
    room_->setExit(Direction::South, "south");
    room_->setExit(Direction::East, "east");
    room_->setExit(Direction::West, "west");
    
    EXPECT_EQ(room_->getExit(Direction::North), "north");
    EXPECT_EQ(room_->getExit(Direction::South), "south");
    EXPECT_EQ(room_->getExit(Direction::East), "east");
    EXPECT_EQ(room_->getExit(Direction::West), "west");
    EXPECT_EQ(room_->getExits().size(), 4);
}

// 测试覆盖出口
TEST_F(RoomTest, OverwriteExit) {
    room_->setExit(Direction::North, "old_room");
    EXPECT_EQ(room_->getExit(Direction::North), "old_room");
    
    room_->setExit(Direction::North, "new_room");
    EXPECT_EQ(room_->getExit(Direction::North), "new_room");
    EXPECT_EQ(room_->getExits().size(), 1); // 应该还是1个出口
}

// 测试NPC管理
TEST_F(RoomTest, NPCManagement) {
    NPC npc("TestNPC", "A test NPC", "Hello!");
    
    EXPECT_TRUE(room_->getNPCs().empty());
    room_->addNPC(npc);
    EXPECT_EQ(room_->getNPCs().size(), 1);
    EXPECT_EQ(room_->getNPCs()[0].name, "TestNPC");
    
    // 移除NPC
    room_->removeNPC("TestNPC");
    EXPECT_TRUE(room_->getNPCs().empty());
}

// 测试多个NPC
TEST_F(RoomTest, MultipleNPCs) {
    room_->addNPC({"NPC1", "First NPC", "Hi"});
    room_->addNPC({"NPC2", "Second NPC", "Hello"});
    room_->addNPC({"NPC3", "Third NPC", "Hey"});
    
    EXPECT_EQ(room_->getNPCs().size(), 3);
    
    room_->removeNPC("NPC2");
    EXPECT_EQ(room_->getNPCs().size(), 2);
}

// 测试怪物管理
TEST_F(RoomTest, MonsterManagement) {
    Monster goblin = Monster::createGoblin();
    
    EXPECT_TRUE(room_->getMonsters().empty());
    room_->addMonster(goblin);
    EXPECT_EQ(room_->getMonsters().size(), 1);
    
    Monster* retrieved = room_->getMonster("Goblin");
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->getName(), "Goblin");
    
    // 移除怪物
    room_->removeMonster("Goblin");
    EXPECT_TRUE(room_->getMonsters().empty());
}

// 测试获取不存在的怪物
TEST_F(RoomTest, GetNonexistentMonster) {
    Monster* monster = room_->getMonster("Dragon");
    EXPECT_EQ(monster, nullptr);
}

// 测试怪物存活状态
TEST_F(RoomTest, MonsterAliveStatus) {
    Monster goblin = Monster::createGoblin();
    room_->addMonster(goblin);
    
    EXPECT_TRUE(room_->hasMonsters());
    
    // 杀死怪物
    Monster* monster = room_->getMonster("Goblin");
    if (monster) {
        monster->takeDamage(1000);
    }
    
    EXPECT_FALSE(room_->hasMonsters()); // 应该没有存活的怪物
}

// 测试多个怪物
TEST_F(RoomTest, MultipleMonsters) {
    room_->addMonster(Monster::createGoblin());
    room_->addMonster(Monster::createWolf());
    room_->addMonster(Monster::createSkeleton());
    
    EXPECT_EQ(room_->getMonsters().size(), 3);
    EXPECT_TRUE(room_->hasMonsters());
    
    room_->removeMonster("Goblin");
    EXPECT_EQ(room_->getMonsters().size(), 2);
}

// 测试方向字符串转换
TEST_F(RoomTest, DirectionToString) {
    EXPECT_EQ(Room::directionToString(Direction::North), "north");
    EXPECT_EQ(Room::directionToString(Direction::South), "south");
    EXPECT_EQ(Room::directionToString(Direction::East), "east");
    EXPECT_EQ(Room::directionToString(Direction::West), "west");
    EXPECT_EQ(Room::directionToString(Direction::Up), "up");
    EXPECT_EQ(Room::directionToString(Direction::Down), "down");
    EXPECT_EQ(Room::directionToString(Direction::Enter), "enter");
    EXPECT_EQ(Room::directionToString(Direction::Exit), "exit");
}

// 测试字符串转方向
TEST_F(RoomTest, StringToDirection) {
    EXPECT_EQ(Room::stringToDirection("north"), Direction::North);
    EXPECT_EQ(Room::stringToDirection("n"), Direction::North);
    EXPECT_EQ(Room::stringToDirection("south"), Direction::South);
    EXPECT_EQ(Room::stringToDirection("s"), Direction::South);
    EXPECT_EQ(Room::stringToDirection("east"), Direction::East);
    EXPECT_EQ(Room::stringToDirection("e"), Direction::East);
    EXPECT_EQ(Room::stringToDirection("west"), Direction::West);
    EXPECT_EQ(Room::stringToDirection("w"), Direction::West);
    EXPECT_EQ(Room::stringToDirection("up"), Direction::Up);
    EXPECT_EQ(Room::stringToDirection("u"), Direction::Up);
    EXPECT_EQ(Room::stringToDirection("down"), Direction::Down);
    EXPECT_EQ(Room::stringToDirection("d"), Direction::Down);
    EXPECT_EQ(Room::stringToDirection("enter"), Direction::Enter);
    EXPECT_EQ(Room::stringToDirection("in"), Direction::Enter);
    EXPECT_EQ(Room::stringToDirection("exit"), Direction::Exit);
    EXPECT_EQ(Room::stringToDirection("out"), Direction::Exit);
    EXPECT_EQ(Room::stringToDirection("invalid"), Direction::None);
}

// 测试World类
TEST_F(RoomTest, WorldManagement) {
    World world;
    
    // 添加房间
    Room room1("room1", "Room 1", "First room");
    room1.setExit(Direction::North, "room2");
    world.addRoom(room1);
    
    Room room2("room2", "Room 2", "Second room");
    room2.setExit(Direction::South, "room1");
    world.addRoom(room2);
    
    // 获取房间
    Room* retrieved = world.getRoom("room1");
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->getName(), "Room 1");
    
    // 移动测试
    std::string next_room = world.move("room1", Direction::North);
    EXPECT_EQ(next_room, "room2");
    
    // 无效移动
    std::string invalid_move = world.move("room1", Direction::East);
    EXPECT_EQ(invalid_move, "");
}

// 测试获取不存在的房间
TEST_F(RoomTest, GetNonexistentRoom) {
    World world;
    Room* room = world.getRoom("nonexistent");
    EXPECT_EQ(room, nullptr);
}

} // namespace mud
