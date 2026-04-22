#include <gtest/gtest.h>
#include "event/EventBus.h"
#include "event/Events.h"
#include <thread>
#include <chrono>

namespace mud {

class EventBusTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 清理之前的订阅
        EventBus::getInstance().clear();
    }

    void TearDown() override {
        EventBus::getInstance().clear();
    }
};

// 测试基本事件发布和订阅
TEST_F(EventBusTest, BasicPublishSubscribe) {
    bool eventReceived = false;
    std::string receivedMessage;

    // 订阅事件
    EventBus::getInstance().subscribe(EventType::MessageSent,
        [&eventReceived, &receivedMessage](const Event& e) {
            eventReceived = true;
            const auto& msgEvent = static_cast<const MessageSentEvent&>(e);
            receivedMessage = msgEvent.message;
        });

    // 发布事件
    MessageSentEvent event(123, "Test message");
    EventBus::getInstance().publish(event);

    // 验证事件被接收
    EXPECT_TRUE(eventReceived);
    EXPECT_EQ(receivedMessage, "Test message");
}

// 测试多个订阅者
TEST_F(EventBusTest, MultipleSubscribers) {
    int subscriber1Count = 0;
    int subscriber2Count = 0;

    // 订阅者1
    EventBus::getInstance().subscribe(EventType::MessageSent,
        [&subscriber1Count](const Event& e) {
            subscriber1Count++;
        });

    // 订阅者2
    EventBus::getInstance().subscribe(EventType::MessageSent,
        [&subscriber2Count](const Event& e) {
            subscriber2Count++;
        });

    // 发布事件
    MessageSentEvent event(123, "Test");
    EventBus::getInstance().publish(event);

    // 验证两个订阅者都收到事件
    EXPECT_EQ(subscriber1Count, 1);
    EXPECT_EQ(subscriber2Count, 1);
}

// 测试不同事件类型
TEST_F(EventBusTest, DifferentEventTypes) {
    bool combatStartedReceived = false;
    bool combatEndedReceived = false;
    bool messageReceived = false;

    // 订阅不同类型的事件
    EventBus::getInstance().subscribe(EventType::CombatStarted,
        [&combatStartedReceived](const Event& e) {
            combatStartedReceived = true;
            const auto& combatEvent = static_cast<const CombatStartedEvent&>(e);
            EXPECT_EQ(combatEvent.monsterName, "Goblin");
        });

    EventBus::getInstance().subscribe(EventType::CombatEnded,
        [&combatEndedReceived](const Event& e) {
            combatEndedReceived = true;
            const auto& combatEvent = static_cast<const CombatEndedEvent&>(e);
            EXPECT_TRUE(combatEvent.playerWon);
        });

    EventBus::getInstance().subscribe(EventType::MessageSent,
        [&messageReceived](const Event& e) {
            messageReceived = true;
        });

    // 发布不同类型的事件
    CombatStartedEvent startEvent(123, "Goblin", "A small green creature");
    EventBus::getInstance().publish(startEvent);

    CombatEndedEvent endEvent(123, true, 10, "Goblin");
    EventBus::getInstance().publish(endEvent);

    MessageSentEvent msgEvent(123, "Hello");
    EventBus::getInstance().publish(msgEvent);

    // 验证
    EXPECT_TRUE(combatStartedReceived);
    EXPECT_TRUE(combatEndedReceived);
    EXPECT_TRUE(messageReceived);
}

// 测试多次发布
TEST_F(EventBusTest, MultiplePublishes) {
    int receiveCount = 0;

    EventBus::getInstance().subscribe(EventType::MessageSent,
        [&receiveCount](const Event& e) {
            receiveCount++;
        });

    // 发布多次
    for (int i = 0; i < 5; i++) {
        MessageSentEvent event(123, "Message " + std::to_string(i));
        EventBus::getInstance().publish(event);
    }

    EXPECT_EQ(receiveCount, 5);
}

// 测试取消订阅
TEST_F(EventBusTest, Unsubscribe) {
    int receiveCount = 0;

    // 订阅
    EventBus::getInstance().subscribe(EventType::MessageSent,
        [&receiveCount](const Event& e) {
            receiveCount++;
        });

    // 发布第一次
    MessageSentEvent event1(123, "First");
    EventBus::getInstance().publish(event1);
    EXPECT_EQ(receiveCount, 1);

    // 取消订阅
    EventBus::getInstance().unsubscribe(EventType::MessageSent);

    // 发布第二次（应该不再接收）
    MessageSentEvent event2(123, "Second");
    EventBus::getInstance().publish(event2);
    EXPECT_EQ(receiveCount, 1); // 仍然是1
}

// 测试玩家攻击事件
TEST_F(EventBusTest, PlayerAttackedEvent) {
    bool eventReceived = false;
    int receivedDamage = 0;
    bool receivedIsCrit = false;
    std::string monsterName;

    EventBus::getInstance().subscribe(EventType::PlayerAttacked,
        [&eventReceived, &receivedDamage, &receivedIsCrit, &monsterName](const Event& e) {
            eventReceived = true;
            const auto& attackEvent = static_cast<const PlayerAttackedEvent&>(e);
            receivedDamage = attackEvent.damage;
            receivedIsCrit = attackEvent.isCrit;
            monsterName = attackEvent.monsterName;
        });

    PlayerAttackedEvent event(123, 25, true, "Dragon");
    EventBus::getInstance().publish(event);

    EXPECT_TRUE(eventReceived);
    EXPECT_EQ(receivedDamage, 25);
    EXPECT_TRUE(receivedIsCrit);
    EXPECT_EQ(monsterName, "Dragon");
}

// 测试怪物攻击事件
TEST_F(EventBusTest, MonsterAttackedEvent) {
    bool eventReceived = false;
    int receivedDamage = 0;
    std::string monsterName;

    EventBus::getInstance().subscribe(EventType::MonsterAttacked,
        [&eventReceived, &receivedDamage, &monsterName](const Event& e) {
            eventReceived = true;
            const auto& attackEvent = static_cast<const MonsterAttackedEvent&>(e);
            receivedDamage = attackEvent.damage;
            monsterName = attackEvent.monsterName;
        });

    MonsterAttackedEvent event(123, 15, "Goblin");
    EventBus::getInstance().publish(event);

    EXPECT_TRUE(eventReceived);
    EXPECT_EQ(receivedDamage, 15);
    EXPECT_EQ(monsterName, "Goblin");
}

// 测试玩家死亡事件
TEST_F(EventBusTest, PlayerDiedEvent) {
    bool eventReceived = false;
    uint32_t sessionId = 0;

    EventBus::getInstance().subscribe(EventType::PlayerDied,
        [&eventReceived, &sessionId](const Event& e) {
            eventReceived = true;
            const auto& deathEvent = static_cast<const PlayerDiedEvent&>(e);
            sessionId = deathEvent.sessionId;
        });

    PlayerDiedEvent event(456);
    EventBus::getInstance().publish(event);

    EXPECT_TRUE(eventReceived);
    EXPECT_EQ(sessionId, 456);
}

// 测试战斗结束事件的完整数据
TEST_F(EventBusTest, CombatEndedEventData) {
    bool eventReceived = false;
    bool playerWon = false;
    int expGained = 0;
    std::string defeatedMonster;

    EventBus::getInstance().subscribe(EventType::CombatEnded,
        [&eventReceived, &playerWon, &expGained, &defeatedMonster](const Event& e) {
            eventReceived = true;
            const auto& combatEvent = static_cast<const CombatEndedEvent&>(e);
            playerWon = combatEvent.playerWon;
            expGained = combatEvent.expGained;
            defeatedMonster = combatEvent.monsterName;
        });

    CombatEndedEvent event(789, true, 50, "Orc");
    EventBus::getInstance().publish(event);

    EXPECT_TRUE(eventReceived);
    EXPECT_TRUE(playerWon);
    EXPECT_EQ(expGained, 50);
    EXPECT_EQ(defeatedMonster, "Orc");
}

// 测试单例模式
TEST_F(EventBusTest, SingletonInstance) {
    EventBus& instance1 = EventBus::getInstance();
    EventBus& instance2 = EventBus::getInstance();

    EXPECT_EQ(&instance1, &instance2);
}

// 测试未订阅事件类型不崩溃
TEST_F(EventBusTest, UnsubscribedEventNoCrash) {
    // 订阅一个事件
    int count = 0;
    EventBus::getInstance().subscribe(EventType::MessageSent,
        [&count](const Event& e) {
            count++;
        });

    // 发布另一个未订阅的事件类型（不应崩溃）
    CombatStartedEvent event(123, "Test", "Test");
    EXPECT_NO_THROW(EventBus::getInstance().publish(event));

    // 只有订阅的事件应该被接收
    EXPECT_EQ(count, 0);
}

} // namespace mud
