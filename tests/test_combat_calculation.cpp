/**
 * @file test_combat_calculation.cpp
 * @brief 战斗计算 API 的 Mock 单元测试
 * 
 * Prompt 演化实验 1: 战斗计算系统测试
 * 覆盖模块：DamageStrategy, CombatManager
 */

#include <gtest/gtest.h>
#include "strategy/DamageStrategy.h"
#include "combat/Combat.h"
#include "combat/Monster.h"
#include "player/Player.h"
#include "event/EventBus.h"

namespace mud {

// ============================================================================
// Prompt 演化实验 1: 战斗计算 API 测试
// ============================================================================

class CombatCalculationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 固定随机种子确保测试可复现
        std::srand(42);
        EventBus::getInstance().clear();
    }

    void TearDown() override {
        EventBus::getInstance().clear();
    }
};

// ----------------------------------------------------------------------------
// 测试组 1: 物理伤害策略公式验证
// ----------------------------------------------------------------------------

/**
 * @brief 验证物理伤害基础公式
 * 
 * 公式：base_damage = attack ± 20%, 防御减免 defense/2
 * 测试思路：使用固定攻击力和防御力，验证伤害在预期范围内
 */
TEST_F(CombatCalculationTest, PhysicalDamageFormulaVerification) {
    PhysicalDamageStrategy strategy;
    bool isCrit = false;

    // 攻击力 100, 防御力 0
    // 预期：80-120 之间 (100 ± 20%)
    for (int i = 0; i < 10; ++i) {
        int damage = strategy.calculateDamage(100, 0, isCrit);
        EXPECT_GE(damage, 80);
        EXPECT_LE(damage, 120);
    }
}

/**
 * @brief 验证物理伤害暴击率和暴击倍数
 * 
 * 公式：暴击率 10%, 暴击伤害 2 倍
 * 测试思路：执行 1000 次攻击，统计暴击次数和暴击伤害
 */
TEST_F(CombatCalculationTest, PhysicalDamageCritRateVerification) {
    PhysicalDamageStrategy strategy;
    
    int critCount = 0;
    int normalDamageSum = 0;
    int critDamageSum = 0;
    int normalCount = 0;

    const int trials = 1000;
    const int attack = 50;
    const int defense = 0;

    for (int i = 0; i < trials; ++i) {
        bool isCrit = false;
        int damage = strategy.calculateDamage(attack, defense, isCrit);
        
        if (isCrit) {
            critCount++;
            critDamageSum += damage;
        } else {
            normalCount++;
            normalDamageSum += damage;
        }
    }

    // 验证暴击率在 5%-15% 之间 (允许统计波动)
    double critRate = static_cast<double>(critCount) / trials;
    EXPECT_GE(critRate, 0.05);
    EXPECT_LE(critRate, 0.15);

    // 验证暴击伤害约为普通伤害的 2 倍
    if (normalCount > 0 && critCount > 0) {
        double avgNormalDamage = static_cast<double>(normalDamageSum) / normalCount;
        double avgCritDamage = static_cast<double>(critDamageSum) / critCount;
        double critMultiplier = avgCritDamage / avgNormalDamage;
        
        EXPECT_GE(critMultiplier, 1.8);  // 允许一定波动
        EXPECT_LE(critMultiplier, 2.2);
    }
}

/**
 * @brief 验证物理伤害防御减免公式
 * 
 * 公式：actual_damage = base_damage - defense/2
 * 测试思路：对比不同防御力下的伤害差值
 */
TEST_F(CombatCalculationTest, PhysicalDamageDefenseReduction) {
    PhysicalDamageStrategy strategy;
    bool isCrit = false;

    const int attack = 100;
    const int defense_low = 0;
    const int defense_high = 60;

    // 收集多次测试结果取平均值
    int damage_low_sum = 0;
    int damage_high_sum = 0;
    const int trials = 100;

    for (int i = 0; i < trials; ++i) {
        damage_low_sum += strategy.calculateDamage(attack, defense_low, isCrit);
        damage_high_sum += strategy.calculateDamage(attack, defense_high, isCrit);
    }

    double avg_damage_low = static_cast<double>(damage_low_sum) / trials;
    double avg_damage_high = static_cast<double>(damage_high_sum) / trials;
    
    // 防御减免应为 defense/2 = 60/2 = 30
    double damage_diff = avg_damage_low - avg_damage_high;
    EXPECT_GE(damage_diff, 25);  // 允许波动
    EXPECT_LE(damage_diff, 35);
}

// ----------------------------------------------------------------------------
// 测试组 2: 魔法伤害策略验证
// ----------------------------------------------------------------------------

/**
 * @brief 验证魔法伤害基础公式
 * 
 * 公式：base_damage = attack ± 30%, 防御减免 defense/4
 */
TEST_F(CombatCalculationTest, MagicDamageFormulaVerification) {
    MagicDamageStrategy strategy;
    bool isCrit = false;

    // 攻击力 100, 防御力 0
    // 预期：70-130 之间 (100 ± 30%)
    for (int i = 0; i < 10; ++i) {
        int damage = strategy.calculateDamage(100, 0, isCrit);
        EXPECT_GE(damage, 70);
        EXPECT_LE(damage, 130);
    }
}

/**
 * @brief 验证魔法伤害暴击率
 * 
 * 公式：暴击率 15%
 */
TEST_F(CombatCalculationTest, MagicDamageCritRateVerification) {
    MagicDamageStrategy strategy;
    
    int critCount = 0;
    const int trials = 1000;

    for (int i = 0; i < trials; ++i) {
        bool isCrit = false;
        strategy.calculateDamage(50, 0, isCrit);
        if (isCrit) {
            critCount++;
        }
    }

    // 验证暴击率在 10%-20% 之间
    double critRate = static_cast<double>(critCount) / trials;
    EXPECT_GE(critRate, 0.10);
    EXPECT_LE(critRate, 0.20);
}

/**
 * @brief 验证魔法伤害穿透效果
 * 
 * 公式：魔法伤害无视部分防御 (defense/4)
 * 测试思路：对比魔法和物理伤害在高防御下的差异
 */
TEST_F(CombatCalculationTest, MagicDamagePenetrationVerification) {
    MagicDamageStrategy magic_strategy;
    PhysicalDamageStrategy physical_strategy;
    bool isCrit = false;

    const int attack = 100;
    const int defense = 80;

    int magic_damage_sum = 0;
    int physical_damage_sum = 0;
    const int trials = 100;

    for (int i = 0; i < trials; ++i) {
        magic_damage_sum += magic_strategy.calculateDamage(attack, defense, isCrit);
        physical_damage_sum += physical_strategy.calculateDamage(attack, defense, isCrit);
    }

    double avg_magic_damage = static_cast<double>(magic_damage_sum) / trials;
    double avg_physical_damage = static_cast<double>(physical_damage_sum) / trials;

    // 魔法伤害应该更高 (因为防御减免更少)
    EXPECT_GT(avg_magic_damage, avg_physical_damage);
    
    // 差值应该约为 defense/2 - defense/4 = 80/4 = 20
    double diff = avg_magic_damage - avg_physical_damage;
    EXPECT_GE(diff, 10);
    EXPECT_LE(diff, 30);
}

// ----------------------------------------------------------------------------
// 测试组 3: 元素伤害策略验证
// ----------------------------------------------------------------------------

/**
 * @brief 验证元素伤害基础公式
 * 
 * 公式：base_damage = attack ± 25%, 防御减免 defense/2
 */
TEST_F(CombatCalculationTest, ElementalDamageFormulaVerification) {
    ElementalDamageStrategy strategy;
    bool isCrit = false;

    // 攻击力 100, 防御力 0
    // 预期：75-125 之间 (100 ± 25%)
    for (int i = 0; i < 10; ++i) {
        int damage = strategy.calculateDamage(100, 0, isCrit);
        EXPECT_GE(damage, 75);
        EXPECT_LE(damage, 125);
    }
}

/**
 * @brief 验证元素伤害暴击率
 * 
 * 公式：暴击率 12%
 */
TEST_F(CombatCalculationTest, ElementalDamageCritRateVerification) {
    ElementalDamageStrategy strategy;
    
    int critCount = 0;
    const int trials = 1000;

    for (int i = 0; i < trials; ++i) {
        bool isCrit = false;
        strategy.calculateDamage(50, 0, isCrit);
        if (isCrit) {
            critCount++;
        }
    }

    // 验证暴击率在 7%-17% 之间
    double critRate = static_cast<double>(critCount) / trials;
    EXPECT_GE(critRate, 0.07);
    EXPECT_LE(critRate, 0.17);
}

// ----------------------------------------------------------------------------
// 测试组 4: 边界条件测试
// ----------------------------------------------------------------------------

/**
 * @brief 测试零攻击力情况
 * 
 * 边界条件：攻击力为 0 时的伤害处理
 */
TEST_F(CombatCalculationTest, ZeroAttackHandling) {
    PhysicalDamageStrategy strategy;
    bool isCrit = false;

    int damage = strategy.calculateDamage(0, 0, isCrit);
    
    // 零攻击力时，伤害应该在 0 附近 (可能因为暴击有微小伤害)
    EXPECT_GE(damage, 0);
    EXPECT_LE(damage, 2);  // 暴击时最多 2 点
}

/**
 * @brief 测试高防御情况下的最小伤害
 * 
 * 边界条件：防御力远大于攻击力时，最小伤害为 1
 */
TEST_F(CombatCalculationTest, HighDefenseMinimumDamage) {
    PhysicalDamageStrategy strategy;
    bool isCrit = false;

    // 攻击力 10, 防御力 100
    // 预期：至少 1 点伤害
    int damage = strategy.calculateDamage(10, 100, isCrit);
    EXPECT_GE(damage, 1);
}

/**
 * @brief 测试精确防御减免计算
 * 
 * 验证防御减免公式的精确性
 */
TEST_F(CombatCalculationTest, DefenseReductionExact) {
    PhysicalDamageStrategy strategy;
    
    // 使用固定值测试，关闭随机性影响
    // 攻击力 50, 防御力 20
    // 基础伤害：50 ± 20% = 40-60
    // 防御减免：20/2 = 10
    // 预期伤害：30-50
    
    const int attack = 50;
    const int defense = 20;
    const int trials = 100;
    
    int total_damage = 0;
    for (int i = 0; i < trials; ++i) {
        bool isCrit = false;
        total_damage += strategy.calculateDamage(attack, defense, isCrit);
    }
    
    double avg_damage = static_cast<double>(total_damage) / trials;
    // 平均基础伤害 50, 减免 10, 预期约 40
    EXPECT_GE(avg_damage, 35);
    EXPECT_LE(avg_damage, 45);
}

/**
 * @brief 测试暴击伤害倍数
 * 
 * 验证暴击时伤害是否为 2 倍
 */
TEST_F(CombatCalculationTest, CriticalHitDamageMultiplier) {
    PhysicalDamageStrategy strategy;
    
    const int attack = 50;
    const int defense = 0;
    
    int normal_damage = 0;
    int crit_damage = 0;
    bool found_crit = false;

    // 寻找一次暴击
    for (int i = 0; i < 1000 && !found_crit; ++i) {
        bool isCrit = false;
        int damage = strategy.calculateDamage(attack, defense, isCrit);
        
        if (isCrit) {
            crit_damage = damage;
            // 再获取一次普通伤害进行比较
            for (int j = 0; j < 10; ++j) {
                bool nc = false;
                normal_damage = strategy.calculateDamage(attack, defense, nc);
                if (!nc) break;
            }
            found_crit = true;
        }
    }

    if (found_crit && normal_damage > 0) {
        double multiplier = static_cast<double>(crit_damage) / normal_damage;
        EXPECT_GE(multiplier, 1.8);
        EXPECT_LE(multiplier, 2.2);
    }
}

// ----------------------------------------------------------------------------
// 测试组 5: CombatManager 集成测试
// ----------------------------------------------------------------------------

/**
 * @brief 测试战斗管理器中的伤害计算集成
 */
TEST_F(CombatCalculationTest, CombatManagerDamageIntegration) {
    // 创建玩家和怪物
    Player player("TestHero");
    player.setLevel(5);
    player.setAttack(30);
    player.setDefense(15);
    player.setCurrentHP(100);

    CombatManager combatManager;
    Monster goblin = Monster::createGoblin();  // HP=30, Attack=15, Defense=5

    combatManager.startCombat(1, goblin);

    // 记录战斗前怪物 HP
    int monsterHPBefore = combatManager.getMonster()->getCurrentHP();

    // 玩家攻击
    CombatResult result = combatManager.playerAttack(
        1, 
        player.getAttack(), 
        player.getDefense(), 
        player.getCurrentHP()
    );

    // 验证伤害已造成
    int monsterHPAfter = combatManager.getMonster()->getCurrentHP();
    int damageDealt = monsterHPBefore - monsterHPAfter;
    
    EXPECT_GT(damageDealt, 0);
    EXPECT_EQ(result.damageDealt, damageDealt);
}

/**
 * @brief 测试不同难度怪物的战斗计算
 */
TEST_F(CombatCalculationTest, CombatCalculationWithDifferentMonsterDifficulties) {
    Player player("TestHero");
    player.setLevel(10);
    player.setAttack(50);
    player.setDefense(25);
    player.setCurrentHP(200);

    // 测试不同难度的怪物
    std::vector<std::pair<Monster, std::string>> monsters = {
        {Monster::createGoblin(), "Goblin(Easy)"},
        {Monster::createOrc(), "Orc(Normal)"},
        {Monster::createDragon(), "Dragon(Boss)"}
    };

    for (const auto& [monster, name] : monsters) {
        CombatManager combatManager;
        combatManager.startCombat(1, monster);

        // 验证怪物属性符合难度设定
        const Monster* m = combatManager.getMonster();
        EXPECT_GT(m->getMaxHP(), 0);
        EXPECT_GT(m->getAttack(), 0);
        EXPECT_GT(m->getDefense(), 0);

        // 执行一次攻击
        CombatResult result = combatManager.playerAttack(
            1,
            player.getAttack(),
            player.getDefense(),
            player.getCurrentHP()
        );

        // 验证战斗结果有效
        EXPECT_GE(result.damageDealt, 0);
        EXPECT_FALSE(result.message.empty());
    }
}

} // namespace mud
