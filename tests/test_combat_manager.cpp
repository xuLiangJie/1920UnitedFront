#include <gtest/gtest.h>
#include "combat/Combat.h"
#include "combat/Monster.h"
#include "event/EventBus.h"
#include "player/Player.h"
#include <memory>

namespace mud {

class CombatManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 清理事件总线
        EventBus::getInstance().clear();

        // 创建玩家
        player_ = std::make_unique<Player>("TestPlayer");
        player_->setLevel(5);
        player_->setAttack(20);
        player_->setDefense(10);
        player_->setCurrentHP(100);

        // 创建战斗管理器
        combat_manager_ = std::make_unique<CombatManager>();
    }

    void TearDown() override {
        EventBus::getInstance().clear();
    }

    std::unique_ptr<Player> player_;
    std::unique_ptr<CombatManager> combat_manager_;
};

// 测试初始战斗状态
TEST_F(CombatManagerTest, InitialState) {
    EXPECT_FALSE(combat_manager_->isInCombat());
    EXPECT_EQ(combat_manager_->getState(), CombatState::NotInCombat);
    EXPECT_EQ(combat_manager_->getMonster(), nullptr);
}

// 测试开始战斗
TEST_F(CombatManagerTest, StartCombat) {
    // 订阅战斗开始事件
    bool combatStartedReceived = false;
    std::string monsterName;
    std::string monsterDesc;

    EventBus::getInstance().subscribe(EventType::CombatStarted,
        [&combatStartedReceived, &monsterName, &monsterDesc](const Event& e) {
            combatStartedReceived = true;
            const auto& event = static_cast<const CombatStartedEvent&>(e);
            monsterName = event.monsterName;
            monsterDesc = event.monsterDescription;
        });

    // 开始战斗
    Monster goblin = Monster::createGoblin();
    combat_manager_->startCombat(123, goblin);

    // 验证战斗状态
    EXPECT_TRUE(combat_manager_->isInCombat());
    EXPECT_EQ(combat_manager_->getState(), CombatState::InCombat);
    ASSERT_NE(combat_manager_->getMonster(), nullptr);
    EXPECT_EQ(combat_manager_->getMonster()->getName(), "Goblin");

    // 验证事件被发布
    EXPECT_TRUE(combatStartedReceived);
    EXPECT_EQ(monsterName, "Goblin");
    EXPECT_FALSE(monsterDesc.empty());
}

// 测试玩家攻击
TEST_F(CombatManagerTest, PlayerAttack) {
    // 开始战斗
    Monster goblin = Monster::createGoblin();
    combat_manager_->startCombat(123, goblin);

    // 订阅玩家攻击事件
    bool playerAttackedReceived = false;
    int damageDealt = 0;
    bool isCrit = false;

    EventBus::getInstance().subscribe(EventType::PlayerAttacked,
        [&playerAttackedReceived, &damageDealt, &isCrit](const Event& e) {
            playerAttackedReceived = true;
            const auto& event = static_cast<const PlayerAttackedEvent&>(e);
            damageDealt = event.damage;
            isCrit = event.isCrit;
        });

    // 玩家攻击
    int playerAttack = player_->getAttack();
    int playerDefense = player_->getDefense();
    int playerHP = player_->getCurrentHP();

    CombatResult result = combat_manager_->playerAttack(123, playerAttack, playerDefense, playerHP);

    // 验证攻击结果
    EXPECT_TRUE(playerAttackedReceived);
    EXPECT_GT(result.damageDealt, 0);
    EXPECT_GE(damageDealt, 0);
    EXPECT_FALSE(result.message.empty());
}

// 测试怪物攻击
TEST_F(CombatManagerTest, MonsterAttack) {
    // 开始战斗
    Monster goblin = Monster::createGoblin();
    combat_manager_->startCombat(123, goblin);

    // 订阅怪物攻击事件
    bool monsterAttackedReceived = false;
    int damageReceived = 0;
    std::string monsterName;

    EventBus::getInstance().subscribe(EventType::MonsterAttacked,
        [&monsterAttackedReceived, &damageReceived, &monsterName](const Event& e) {
            monsterAttackedReceived = true;
            const auto& event = static_cast<const MonsterAttackedEvent&>(e);
            damageReceived = event.damage;
            monsterName = event.monsterName;
        });

    // 怪物攻击
    int playerDefense = player_->getDefense();
    int playerHP = player_->getCurrentHP();

    CombatResult result = combat_manager_->monsterAttack(123, playerDefense, playerHP);

    // 验证攻击结果
    EXPECT_TRUE(monsterAttackedReceived);
    EXPECT_GT(result.damageDealt, 0);
    EXPECT_GT(damageReceived, 0);
    EXPECT_EQ(monsterName, "Goblin");
    EXPECT_FALSE(result.message.empty());
}

// 测试战斗胜利
TEST_F(CombatManagerTest, CombatVictory) {
    // 开始战斗
    Monster goblin = Monster::createGoblin();
    combat_manager_->startCombat(123, goblin);

    // 订阅战斗结束事件
    bool combatEndedReceived = false;
    bool playerWon = false;
    int expGained = 0;

    EventBus::getInstance().subscribe(EventType::CombatEnded,
        [&combatEndedReceived, &playerWon, &expGained](const Event& e) {
            combatEndedReceived = true;
            const auto& event = static_cast<const CombatEndedEvent&>(e);
            playerWon = event.playerWon;
            expGained = event.expGained;
        });

    // 直接杀死怪物（设置HP为0）
    combat_manager_->getMonster()->setCurrentHP(0);

    // 玩家攻击，触发战斗结束
    int playerAttack = player_->getAttack();
    int playerDefense = player_->getDefense();
    int playerHP = player_->getCurrentHP();

    CombatResult result = combat_manager_->playerAttack(123, playerAttack, playerDefense, playerHP);

    // 验证胜利
    EXPECT_TRUE(result.playerWon);
    EXPECT_GT(result.expGained, 0);
    EXPECT_NE(result.message.find("VICTORY"), std::string::npos);
    EXPECT_TRUE(combatEndedReceived);
    EXPECT_TRUE(playerWon);
    EXPECT_GT(expGained, 0);
}

// 测试战斗失败
TEST_F(CombatManagerTest, CombatDefeat) {
    // 开始战斗
    Monster goblin = Monster::createGoblin();
    combat_manager_->startCombat(123, goblin);

    // 订阅战斗结束和玩家死亡事件
    bool combatEndedReceived = false;
    bool playerDiedReceived = false;
    bool playerWon = false;

    EventBus::getInstance().subscribe(EventType::CombatEnded,
        [&combatEndedReceived, &playerWon](const Event& e) {
            combatEndedReceived = true;
            const auto& event = static_cast<const CombatEndedEvent&>(e);
            playerWon = event.playerWon;
        });

    EventBus::getInstance().subscribe(EventType::PlayerDied,
        [&playerDiedReceived](const Event& e) {
            playerDiedReceived = true;
        });

    // 设置玩家HP为0，模拟死亡
    int playerAttack = player_->getAttack();
    int playerDefense = player_->getDefense();
    int playerHP = 0; // 玩家已经死亡

    CombatResult result = combat_manager_->playerAttack(123, playerAttack, playerDefense, playerHP);

    // 验证失败
    EXPECT_FALSE(result.playerWon);
    EXPECT_TRUE(combatEndedReceived);
    EXPECT_FALSE(playerWon);
    EXPECT_TRUE(playerDiedReceived);
}

// 测试战斗状态描述
TEST_F(CombatManagerTest, GetCombatStatus) {
    // 未开始战斗
    std::string status = combat_manager_->getCombatStatus();
    EXPECT_NE(status.find("not in combat"), std::string::npos);

    // 开始战斗
    Monster goblin = Monster::createGoblin();
    combat_manager_->startCombat(123, goblin);

    // 战斗中
    status = combat_manager_->getCombatStatus();
    EXPECT_NE(status.find("fighting"), std::string::npos);
    EXPECT_NE(status.find("Goblin"), std::string::npos);
}

// 测试不在战斗中攻击
TEST_F(CombatManagerTest, AttackWhenNotInCombat) {
    // 不开始战斗直接攻击
    int playerAttack = player_->getAttack();
    int playerDefense = player_->getDefense();
    int playerHP = player_->getCurrentHP();

    CombatResult result = combat_manager_->playerAttack(123, playerAttack, playerDefense, playerHP);

    // 应该返回错误信息
    EXPECT_FALSE(result.playerWon);
    EXPECT_EQ(result.damageDealt, 0);
    EXPECT_NE(result.message.find("not in combat"), std::string::npos);
}

// 测试结束战斗
TEST_F(CombatManagerTest, EndCombat) {
    // 开始战斗
    Monster goblin = Monster::createGoblin();
    combat_manager_->startCombat(123, goblin);

    EXPECT_TRUE(combat_manager_->isInCombat());

    // 订阅战斗结束事件
    bool combatEndedReceived = false;
    EventBus::getInstance().subscribe(EventType::CombatEnded,
        [&combatEndedReceived](const Event& e) {
            combatEndedReceived = true;
        });

    // 结束战斗（胜利）
    combat_manager_->endCombat(123, true);

    // 验证战斗结束
    EXPECT_FALSE(combat_manager_->isInCombat());
    EXPECT_EQ(combat_manager_->getState(), CombatState::CombatWon);
    EXPECT_TRUE(combatEndedReceived);
}

// 测试连续攻击直到胜利
TEST_F(CombatManagerTest, ContinuousAttacksUntilVictory) {
    // 开始战斗
    Monster goblin = Monster::createGoblin();
    combat_manager_->startCombat(123, goblin);

    // 订阅事件
    int attackCount = 0;
    bool victoryReceived = false;

    EventBus::getInstance().subscribe(EventType::PlayerAttacked,
        [&attackCount](const Event& e) {
            attackCount++;
        });

    EventBus::getInstance().subscribe(EventType::CombatEnded,
        [&victoryReceived](const Event& e) {
            const auto& event = static_cast<const CombatEndedEvent&>(e);
            if (event.playerWon) {
                victoryReceived = true;
            }
        });

    // 连续攻击直到胜利
    int playerAttack = player_->getAttack() * 10; // 增加攻击力加速
    int playerDefense = player_->getDefense();
    int playerHP = player_->getCurrentHP();

    while (combat_manager_->isInCombat() && attackCount < 10) {
        combat_manager_->playerAttack(123, playerAttack, playerDefense, playerHP);
    }

    // 应该在有限的攻击次数内获得胜利
    EXPECT_GT(attackCount, 0);
    EXPECT_TRUE(victoryReceived);
    EXPECT_FALSE(combat_manager_->isInCombat());
}

// 测试不同怪物类型
TEST_F(CombatManagerTest, DifferentMonsterTypes) {
    std::vector<Monster> monsters = {
        Monster::createGoblin(),
        Monster::createWolf(),
        Monster::createSkeleton(),
        Monster::createOrc()
    };

    for (auto& monster : monsters) {
        // 开始战斗
        combat_manager_->startCombat(123, monster);

        // 验证怪物信息
        EXPECT_TRUE(combat_manager_->isInCombat());
        ASSERT_NE(combat_manager_->getMonster(), nullptr);
        EXPECT_EQ(combat_manager_->getMonster()->getName(), monster.getName());

        // 结束战斗
        combat_manager_->endCombat(123, true);
        EXPECT_FALSE(combat_manager_->isInCombat());
    }
}

// 测试暴击机制
TEST_F(CombatManagerTest, CriticalHit) {
    // 开始战斗
    Monster goblin = Monster::createGoblin();
    combat_manager_->startCombat(123, goblin);

    // 订阅攻击事件
    bool critReceived = false;
    int normalDamage = 0;
    int critDamage = 0;

    EventBus::getInstance().subscribe(EventType::PlayerAttacked,
        [&critReceived, &normalDamage, &critDamage](const Event& e) {
            const auto& event = static_cast<const PlayerAttackedEvent&>(e);
            if (event.isCrit) {
                critReceived = true;
                critDamage = event.damage;
            } else {
                normalDamage = event.damage;
            }
        });

    // 多次攻击，尝试触发暴击
    int playerAttack = player_->getAttack();
    int playerDefense = player_->getDefense();
    int playerHP = player_->getCurrentHP();

    for (int i = 0; i < 50; i++) {
        // 重置怪物HP
        combat_manager_->getMonster()->setCurrentHP(combat_manager_->getMonster()->getMaxHP());
        combat_manager_->playerAttack(123, playerAttack, playerDefense, playerHP);

        if (critReceived) break;
    }

    // 暴击伤害应该大于普通伤害
    if (critReceived && normalDamage > 0) {
        EXPECT_GT(critDamage, normalDamage);
    }
}

// 测试防御减免
TEST_F(CombatManagerTest, DefenseReduction) {
    // 开始战斗
    Monster goblin = Monster::createGoblin();
    combat_manager_->startCombat(123, goblin);

    // 设置高防御
    int highDefense = 50;
    int lowDefense = 5;

    int playerAttack = player_->getAttack();
    int playerHP = player_->getCurrentHP();

    // 测试高防御
    CombatResult result1 = combat_manager_->monsterAttack(123, highDefense, playerHP);
    int damageWithHighDefense = result1.damageDealt;

    // 重置怪物HP
    combat_manager_->getMonster()->setCurrentHP(combat_manager_->getMonster()->getMaxHP());

    // 测试低防御
    CombatResult result2 = combat_manager_->monsterAttack(123, lowDefense, playerHP);
    int damageWithLowDefense = result2.damageDealt;

    // 高防御应该受到更少伤害
    EXPECT_LT(damageWithHighDefense, damageWithLowDefense);
}

// 测试战斗状态机转换
TEST_F(CombatManagerTest, CombatStateMachine) {
    // 初始状态
    EXPECT_EQ(combat_manager_->getState(), CombatState::NotInCombat);

    // 开始战斗
    Monster goblin = Monster::createGoblin();
    combat_manager_->startCombat(123, goblin);
    EXPECT_EQ(combat_manager_->getState(), CombatState::InCombat);

    // 胜利
    combat_manager_->endCombat(123, true);
    EXPECT_EQ(combat_manager_->getState(), CombatState::CombatWon);

    // 重置
    combat_manager_ = std::make_unique<CombatManager>();
    combat_manager_->startCombat(123, goblin);
    EXPECT_EQ(combat_manager_->getState(), CombatState::InCombat);

    // 失败
    combat_manager_->endCombat(123, false);
    EXPECT_EQ(combat_manager_->getState(), CombatState::CombatLost);
}

} // namespace mud
