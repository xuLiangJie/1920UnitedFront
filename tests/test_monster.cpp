#include <gtest/gtest.h>
#include "combat/Monster.h"

namespace mud {

class MonsterTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::srand(42); // 固定随机种子以便测试
    }
};

// 测试默认构造函数
TEST_F(MonsterTest, DefaultConstructor) {
    Monster monster;
    EXPECT_EQ(monster.getName(), "");
    EXPECT_EQ(monster.getMaxHP(), 10);
    EXPECT_EQ(monster.getCurrentHP(), 10);
    EXPECT_EQ(monster.getAttack(), 5);
    EXPECT_EQ(monster.getDefense(), 0);
    EXPECT_EQ(monster.getExpReward(), 10);
    EXPECT_TRUE(monster.isAlive());
    EXPECT_FALSE(monster.isDead());
}

// 测试哥布林创建
TEST_F(MonsterTest, CreateGoblin) {
    Monster goblin = Monster::createGoblin();
    EXPECT_EQ(goblin.getName(), "Goblin");
    EXPECT_NE(goblin.getDescription().find("green"), std::string::npos);
    EXPECT_EQ(goblin.getLevel(), MonsterLevel::Easy);
    EXPECT_TRUE(goblin.isAlive());
    EXPECT_GT(goblin.getExpReward(), 0);
}

// 测试野狼创建
TEST_F(MonsterTest, CreateWolf) {
    Monster wolf = Monster::createWolf();
    EXPECT_EQ(wolf.getName(), "Wild Wolf");
    EXPECT_NE(wolf.getDescription().find("wolf"), std::string::npos);
    EXPECT_EQ(wolf.getLevel(), MonsterLevel::Easy);
}

// 测试骷髅创建
TEST_F(MonsterTest, CreateSkeleton) {
    Monster skeleton = Monster::createSkeleton();
    EXPECT_EQ(skeleton.getName(), "Skeleton Warrior");
    EXPECT_EQ(skeleton.getLevel(), MonsterLevel::Normal);
    EXPECT_GT(skeleton.getAttack(), 5); // 应该比简单怪物强
}

// 测试兽人创建
TEST_F(MonsterTest, CreateOrc) {
    Monster orc = Monster::createOrc();
    EXPECT_EQ(orc.getName(), "Orc Berserker");
    EXPECT_EQ(orc.getLevel(), MonsterLevel::Hard);
    EXPECT_GT(orc.getMaxHP(), 60); // 应该比普通怪物血多
}

// 测试龙创建
TEST_F(MonsterTest, CreateDragon) {
    Monster dragon = Monster::createDragon();
    EXPECT_EQ(dragon.getName(), "Young Dragon");
    EXPECT_EQ(dragon.getLevel(), MonsterLevel::Boss);
    EXPECT_GT(dragon.getMaxHP(), 100); // Boss应该血很多
    EXPECT_GT(dragon.getExpReward(), 100);
}

// 测试怪物攻击
TEST_F(MonsterTest, MonsterAttack) {
    Monster goblin = Monster::createGoblin();
    int damage = goblin.attack();
    EXPECT_GT(damage, 0);
    EXPECT_LE(damage, goblin.getAttack() * 2); // 伤害应该在合理范围内
}

// 测试怪物受伤
TEST_F(MonsterTest, TakeDamage) {
    Monster goblin = Monster::createGoblin();
    int initial_hp = goblin.getCurrentHP();
    
    goblin.takeDamage(10);
    EXPECT_LT(goblin.getCurrentHP(), initial_hp);
    
    int expected_reduction = std::max(1, 10 - goblin.getDefense());
    EXPECT_EQ(goblin.getCurrentHP(), initial_hp - expected_reduction);
}

// 测试怪物死亡
TEST_F(MonsterTest, MonsterDeath) {
    Monster goblin = Monster::createGoblin();
    goblin.takeDamage(1000); // 大量伤害
    EXPECT_FALSE(goblin.isAlive());
    EXPECT_TRUE(goblin.isDead());
}

// 测试治疗
TEST_F(MonsterTest, Heal) {
    Monster goblin = Monster::createGoblin();
    int max_hp = goblin.getMaxHP();
    
    goblin.takeDamage(50);
    int damaged_hp = goblin.getCurrentHP();
    
    goblin.heal(20);
    EXPECT_GT(goblin.getCurrentHP(), damaged_hp);
}

// 测试治疗不超过最大HP
TEST_F(MonsterTest, HealOverMax) {
    Monster goblin = Monster::createGoblin();
    int max_hp = goblin.getMaxHP();
    
    goblin.heal(1000); // 大量治疗
    EXPECT_EQ(goblin.getCurrentHP(), max_hp); // 不应该超过最大值
}

// 测试健康状态显示
TEST_F(MonsterTest, HealthStatus) {
    Monster goblin = Monster::createGoblin();
    
    // 健康状态
    std::string healthy_status = goblin.getHealthStatus();
    EXPECT_NE(healthy_status.find("healthy"), std::string::npos);
    
    // 受伤状态
    goblin.takeDamage(goblin.getMaxHP() / 2);
    std::string wounded_status = goblin.getHealthStatus();
    EXPECT_NE(wounded_status, healthy_status);
}

// 测试死亡消息
TEST_F(MonsterTest, DeathMessage) {
    Monster goblin = Monster::createGoblin();
    std::string death_msg = goblin.getDeathMessage();
    EXPECT_NE(death_msg.find("Goblin"), std::string::npos);
    EXPECT_NE(death_msg.find("collapses"), std::string::npos);
}

// 测试不同难度怪物的属性差异
TEST_F(MonsterTest, DifficultyScaling) {
    Monster easy = Monster::createGoblin();
    Monster normal = Monster::createSkeleton();
    Monster hard = Monster::createOrc();
    Monster boss = Monster::createDragon();
    
    // Boss应该是最强的
    EXPECT_GT(boss.getMaxHP(), hard.getMaxHP());
    EXPECT_GT(boss.getMaxHP(), normal.getMaxHP());
    EXPECT_GT(boss.getMaxHP(), easy.getMaxHP());
    
    EXPECT_GT(boss.getAttack(), hard.getAttack());
    EXPECT_GT(boss.getExpReward(), hard.getExpReward());
}

} // namespace mud
