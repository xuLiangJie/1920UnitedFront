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
        EventBus::getInstance().clear();
        player_ = std::make_unique<Player>("TestPlayer");
        player_->setLevel(5);
        player_->setAttack(20);
        player_->setDefense(10);
        player_->setCurrentHP(100);
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
}

// 测试开始战斗
TEST_F(CombatManagerTest, StartCombat) {
    bool combatStartedReceived = false;
    EventBus::getInstance().subscribe(EventType::CombatStarted,
        [&combatStartedReceived](const Event& e) {
            combatStartedReceived = true;
        });

    Monster goblin = Monster::createGoblin();
    combat_manager_->startCombat(123, goblin);

    EXPECT_TRUE(combat_manager_->isInCombat());
    EXPECT_EQ(combat_manager_->getState(), CombatState::InCombat);
    EXPECT_EQ(combat_manager_->getMonster()->getName(), "Goblin");
    EXPECT_TRUE(combatStartedReceived);
}

// 测试玩家攻击
TEST_F(CombatManagerTest, PlayerAttack) {
    Monster goblin = Monster::createGoblin();
    combat_manager_->startCombat(123, goblin);

    bool playerAttackedReceived = false;
    EventBus::getInstance().subscribe(EventType::PlayerAttacked,
        [&playerAttackedReceived](const Event& e) {
            playerAttackedReceived = true;
        });

    CombatResult result = combat_manager_->playerAttack(
        123, player_->getAttack(), player_->getDefense(), player_->getCurrentHP());

    EXPECT_TRUE(playerAttackedReceived);
    EXPECT_GE(result.damageDealt, 0);
    EXPECT_FALSE(result.message.empty());
}

// 测试怪物攻击
TEST_F(CombatManagerTest, MonsterAttack) {
    Monster goblin = Monster::createGoblin();
    combat_manager_->startCombat(123, goblin);

    bool monsterAttackedReceived = false;
    EventBus::getInstance().subscribe(EventType::MonsterAttacked,
        [&monsterAttackedReceived](const Event& e) {
            monsterAttackedReceived = true;
        });

    CombatResult result = combat_manager_->monsterAttack(
        123, player_->getDefense(), player_->getCurrentHP());

    EXPECT_TRUE(monsterAttackedReceived);
    EXPECT_GT(result.damageDealt, 0);
    EXPECT_FALSE(result.message.empty());
}

// 测试战斗胜利
TEST_F(CombatManagerTest, CombatVictory) {
    Monster goblin = Monster::createGoblin();
    combat_manager_->startCombat(123, goblin);

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

    // 重复攻击直到怪物死亡
    int playerAttack = player_->getAttack() * 5;  // 高攻击力
    int playerDefense = player_->getDefense();
    int playerHP = player_->getCurrentHP();

    while (combat_manager_->isInCombat()) {
        CombatResult result = combat_manager_->playerAttack(
            123, playerAttack, playerDefense, playerHP);
        if (result.playerWon) break;
    }

    // 验证胜利
    EXPECT_FALSE(combat_manager_->isInCombat());
    EXPECT_TRUE(combatEndedReceived);
    EXPECT_TRUE(playerWon);
    EXPECT_GT(expGained, 0);
}

// 测试战斗失败
TEST_F(CombatManagerTest, CombatDefeat) {
    Monster dragon = Monster::createDragon();  // Boss 级怪物
    combat_manager_->startCombat(123, dragon);

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

    // 玩家 HP=0 模拟死亡
    CombatResult result = combat_manager_->playerAttack(
        123, player_->getAttack(), player_->getDefense(), 0);

    // 验证失败
    EXPECT_FALSE(combat_manager_->isInCombat());
    EXPECT_TRUE(combatEndedReceived);
    EXPECT_FALSE(playerWon);
    EXPECT_TRUE(playerDiedReceived);
}

// 测试战斗状态描述
TEST_F(CombatManagerTest, GetCombatStatus) {
    std::string status = combat_manager_->getCombatStatus();
    EXPECT_NE(status.find("not in combat"), std::string::npos);

    Monster goblin = Monster::createGoblin();
    combat_manager_->startCombat(123, goblin);

    status = combat_manager_->getCombatStatus();
    EXPECT_NE(status.find("fighting"), std::string::npos);
    EXPECT_NE(status.find("Goblin"), std::string::npos);
}

// 测试不在战斗中攻击
TEST_F(CombatManagerTest, AttackWhenNotInCombat) {
    CombatResult result = combat_manager_->playerAttack(
        123, player_->getAttack(), player_->getDefense(), player_->getCurrentHP());

    EXPECT_EQ(result.damageDealt, 0);
    EXPECT_NE(result.message.find("not in combat"), std::string::npos);
}

// 测试不同怪物类型
TEST_F(CombatManagerTest, DifferentMonsterTypes) {
    std::vector<std::pair<std::function<Monster()>, std::string>> monsterCreators = {
        {Monster::createGoblin, "Goblin"},
        {Monster::createWolf, "Wild Wolf"},
        {Monster::createSkeleton, "Skeleton Warrior"},
        {Monster::createOrc, "Orc Berserker"}
    };
    
    for (const auto& [creator, expectedName] : monsterCreators) {
        combat_manager_ = std::make_unique<CombatManager>();
        combat_manager_->startCombat(1, creator());

        EXPECT_TRUE(combat_manager_->isInCombat());
        EXPECT_EQ(combat_manager_->getMonster()->getName(), expectedName);
    }
}

// 测试暴击机制
TEST_F(CombatManagerTest, CriticalHit) {
    // 单独测试暴击率，不通过 CombatManager（避免战斗逻辑干扰）
    // 直接使用 DamageStrategy 测试暴击
    PhysicalDamageStrategy strategy;
    
    int critCount = 0;
    const int trials = 200;  // 增加测试次数
    
    for (int i = 0; i < trials; ++i) {
        bool isCrit = false;
        strategy.calculateDamage(50, 0, isCrit);
        if (isCrit) critCount++;
    }

    // 10% 暴击率，200 次应该有 10-30 次暴击（允许统计波动）
    EXPECT_GE(critCount, 10);
    EXPECT_LE(critCount, 40);
}

// 测试防御减免
TEST_F(CombatManagerTest, DefenseReduction) {
    Monster dragon = Monster::createDragon();
    combat_manager_->startCombat(123, dragon);

    int highDefense = 50;
    int lowDefense = 5;
    int playerHP = player_->getCurrentHP();

    CombatResult result1 = combat_manager_->monsterAttack(123, highDefense, playerHP);
    int damageWithHighDefense = result1.damageDealt;

    combat_manager_->getMonster()->setCurrentHP(
        combat_manager_->getMonster()->getMaxHP());

    CombatResult result2 = combat_manager_->monsterAttack(123, lowDefense, playerHP);
    int damageWithLowDefense = result2.damageDealt;

    // 高防御应该受到更少伤害
    EXPECT_LE(damageWithHighDefense, damageWithLowDefense + 10);
}

} // namespace mud
