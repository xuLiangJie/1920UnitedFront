#include <gtest/gtest.h>
#include "strategy/DamageStrategy.h"

namespace mud {

class DamageStrategyTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::srand(42); // 固定随机种子
    }
};

// 测试物理伤害策略
TEST_F(DamageStrategyTest, PhysicalDamageBasic) {
    PhysicalDamageStrategy strategy;
    bool isCrit = false;

    int damage = strategy.calculateDamage(10, 5, isCrit);

    EXPECT_GT(damage, 0);
    EXPECT_LE(damage, 20); // 应该在合理范围内
}

// 测试物理伤害暴击
TEST_F(DamageStrategyTest, PhysicalDamageCrit) {
    PhysicalDamageStrategy strategy;

    // 多次测试以触发暴击（10% 概率）
    bool gotCrit = false;
    for (int i = 0; i < 100; ++i) {
        bool isCrit = false;
        strategy.calculateDamage(10, 0, isCrit);
        if (isCrit) {
            gotCrit = true;
            break;
        }
    }

    // 100 次尝试应该至少有一次暴击
    EXPECT_TRUE(gotCrit);
}

// 测试物理伤害防御减免
TEST_F(DamageStrategyTest, PhysicalDamageDefense) {
    PhysicalDamageStrategy strategy;
    bool isCrit = false;

    int damage_low_def = strategy.calculateDamage(20, 0, isCrit);
    int damage_high_def = strategy.calculateDamage(20, 20, isCrit);

    // 高防御应该造成更少伤害
    EXPECT_GT(damage_low_def, damage_high_def);
}

// 测试物理伤害最小值
TEST_F(DamageStrategyTest, PhysicalDamageMinimum) {
    PhysicalDamageStrategy strategy;
    bool isCrit = false;

    // 高防御应该至少造成 1 点伤害
    int damage = strategy.calculateDamage(5, 100, isCrit);
    EXPECT_GE(damage, 1);
}

// 测试魔法伤害策略
TEST_F(DamageStrategyTest, MagicDamageBasic) {
    MagicDamageStrategy strategy;
    bool isCrit = false;

    int damage = strategy.calculateDamage(10, 5, isCrit);

    EXPECT_GT(damage, 0);
}

// 测试魔法伤害波动更大
TEST_F(DamageStrategyTest, MagicDamageVariance) {
    MagicDamageStrategy magic_strategy;
    PhysicalDamageStrategy physical_strategy;
    bool isCrit = false;

    int magic_min = 1000, magic_max = 0;
    int physical_min = 1000, physical_max = 0;

    // 统计伤害范围
    for (int i = 0; i < 100; ++i) {
        int magic_dmg = magic_strategy.calculateDamage(20, 0, isCrit);
        int physical_dmg = physical_strategy.calculateDamage(20, 0, isCrit);

        magic_min = std::min(magic_min, magic_dmg);
        magic_max = std::max(magic_max, magic_dmg);
        physical_min = std::min(physical_min, physical_dmg);
        physical_max = std::max(physical_max, physical_dmg);
    }

    // 魔法伤害波动应该更大（相对而言）
    int magic_range = magic_max - magic_min;
    int physical_range = physical_max - physical_min;
    EXPECT_GE(magic_range, physical_range);
}

// 测试魔法伤害无视部分防御
TEST_F(DamageStrategyTest, MagicDamageDefensePenetration) {
    MagicDamageStrategy magic_strategy;
    PhysicalDamageStrategy physical_strategy;
    bool isCrit = false;

    int magic_damage = magic_strategy.calculateDamage(20, 20, isCrit);
    int physical_damage = physical_strategy.calculateDamage(20, 20, isCrit);

    // 魔法伤害应该更高（因为无视部分防御）
    EXPECT_GE(magic_damage, physical_damage);
}

// 测试元素伤害策略
TEST_F(DamageStrategyTest, ElementalDamageBasic) {
    ElementalDamageStrategy strategy;
    bool isCrit = false;

    int damage = strategy.calculateDamage(10, 5, isCrit);

    EXPECT_GT(damage, 0);
}

// 测试元素伤害暴击率
TEST_F(DamageStrategyTest, ElementalDamageCritRate) {
    ElementalDamageStrategy strategy;

    int crit_count = 0;
    for (int i = 0; i < 100; ++i) {
        bool isCrit = false;
        strategy.calculateDamage(10, 0, isCrit);
        if (isCrit) {
            crit_count++;
        }
    }

    // 12% 暴击率，100 次应该有约 12 次暴击
    EXPECT_GT(crit_count, 5); // 至少 5 次
    EXPECT_LT(crit_count, 25); // 最多 25 次
}

// 测试元素伤害无视一半防御 - 使用平均值减少随机波动
TEST_F(DamageStrategyTest, ElementalDamageHalfDefense) {
    ElementalDamageStrategy strategy;
    
    // 多次测试取平均值以减少随机波动影响
    int total_diff = 0;
    const int trials = 100;
    
    for (int i = 0; i < trials; ++i) {
        bool isCrit = false;
        int no_defense = strategy.calculateDamage(100, 0, isCrit);
        int with_defense = strategy.calculateDamage(100, 50, isCrit);
        total_diff += (no_defense - with_defense);
    }
    
    // 平均差值应该大约是防御的一半 (50/2 = 25)，允许波动
    double avg_diff = static_cast<double>(total_diff) / trials;
    EXPECT_GE(avg_diff, 15);  // 至少 15
    EXPECT_LE(avg_diff, 35);  // 最多 35
}

// 测试策略名称
TEST_F(DamageStrategyTest, StrategyNames) {
    PhysicalDamageStrategy physical;
    MagicDamageStrategy magic;
    ElementalDamageStrategy elemental;

    EXPECT_EQ(physical.getStrategyName(), "Physical");
    EXPECT_EQ(magic.getStrategyName(), "Magic");
    EXPECT_EQ(elemental.getStrategyName(), "Elemental");
}

// 测试零攻击力
TEST_F(DamageStrategyTest, ZeroAttack) {
    PhysicalDamageStrategy strategy;
    bool isCrit = false;

    int damage = strategy.calculateDamage(0, 0, isCrit);
    EXPECT_GE(damage, 0);
}

// 测试高攻击力
TEST_F(DamageStrategyTest, HighAttack) {
    PhysicalDamageStrategy strategy;
    bool isCrit = false;

    int damage = strategy.calculateDamage(1000, 0, isCrit);
    EXPECT_GT(damage, 500); // 应该造成大量伤害
    EXPECT_LE(damage, 2000); // 但不应该超过 2 倍（暴击）
}

} // namespace mud
