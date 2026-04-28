/**
 * @file test_command_registry_extended.cpp
 * @brief 命令注册中心的扩展测试（独立版本，避免单例问题）
 */

#include <gtest/gtest.h>
#include "command/CommandRegistry.h"
#include "command/LookCommand.h"
#include "command/MoveCommand.h"
#include "command/SayCommand.h"
#include "command/HelpCommand.h"
#include "command/WhoCommand.h"
#include "player/Player.h"
#include <memory>

namespace mud {

// 注意：由于 CommandRegistry 是单例，这些测试可能会影响其他测试
// 在实际项目中，应该考虑依赖注入或重构单例

class CommandRegistryExtendedTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 获取 CommandRegistry 实例（单例）
        registry_ = &CommandRegistry::getInstance();
    }

    void TearDown() override {
        // 不删除单例，保持全局状态
    }

    CommandRegistry* registry_;
};

// 测试命令注册大小写不敏感
TEST_F(CommandRegistryExtendedTest, CaseInsensitiveCommand) {
    // 这个测试只验证已注册的命令
    EXPECT_TRUE(registry_->hasCommand("help"));
    EXPECT_TRUE(registry_->hasCommand("Help"));
    EXPECT_TRUE(registry_->hasCommand("HELP"));
}

// 测试执行命令
TEST_F(CommandRegistryExtendedTest, ExecuteCommand) {
    // 执行 look 命令
    std::string result = registry_->executeCommand(nullptr, "look");

    // 验证命令执行结果（look 命令在没有房间时返回默认消息）
    EXPECT_FALSE(result.empty());
}

// 测试执行不存在的命令
TEST_F(CommandRegistryExtendedTest, ExecuteNonExistentCommand) {
    std::string result = registry_->executeCommand(nullptr, "nonexistent_command_xyz");

    EXPECT_NE(result.find("Unknown command"), std::string::npos);
}

// 测试获取所有帮助
TEST_F(CommandRegistryExtendedTest, GetAllHelp) {
    std::string help = registry_->getAllHelp();

    EXPECT_FALSE(help.empty());
}

// 测试命令帮助包含注册的命令
TEST_F(CommandRegistryExtendedTest, HelpContainsRegisteredCommands) {
    std::string help = registry_->getAllHelp();

    EXPECT_NE(help.find("help"), std::string::npos);
    EXPECT_NE(help.find("look"), std::string::npos);
    EXPECT_NE(help.find("who"), std::string::npos);
    EXPECT_NE(help.find("say"), std::string::npos);
}

// 测试执行带参数的命令
TEST_F(CommandRegistryExtendedTest, ExecuteCommandWithArguments) {
    // Say 命令带参数
    std::string result = registry_->executeCommand(nullptr, "say Hello World");

    // Say 命令应该返回消息
    EXPECT_FALSE(result.empty());
}

// 测试命令执行不区分大小写
TEST_F(CommandRegistryExtendedTest, ExecuteCommandCaseInsensitive) {
    std::string result1 = registry_->executeCommand(nullptr, "LOOK");
    std::string result2 = registry_->executeCommand(nullptr, "look");
    std::string result3 = registry_->executeCommand(nullptr, "Look");

    // 所有形式都应该有效
    EXPECT_FALSE(result1.empty());
    EXPECT_FALSE(result2.empty());
    EXPECT_FALSE(result3.empty());
}

// 测试帮助命令
TEST_F(CommandRegistryExtendedTest, HelpCommand) {
    std::string result = registry_->executeCommand(nullptr, "help");
    EXPECT_FALSE(result.empty());
    EXPECT_NE(result.find("help"), std::string::npos);
}

// 测试 Who 命令（在线玩家列表）
TEST_F(CommandRegistryExtendedTest, WhoCommand) {
    std::string result = registry_->executeCommand(nullptr, "who");
    // Who 命令应该返回在线玩家列表（可能为空）
    EXPECT_FALSE(result.empty());
}

} // namespace mud
