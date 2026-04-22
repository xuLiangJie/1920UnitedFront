#include <gtest/gtest.h>
#include "command/CommandRegistry.h"
#include "command/LookCommand.h"
#include "command/MoveCommand.h"
#include "command/SayCommand.h"
#include "command/HelpCommand.h"
#include "command/WhoCommand.h"
#include "server/Session.h"
#include "server/MudServer.h"
#include "player/Player.h"
#include <memory>

namespace mud {

// Mock Session for testing
class MockSession {
public:
    MockSession() : player_() {}
    Player& getPlayer() { return player_; }
    const Player& getPlayer() const { return player_; }
    std::vector<std::string> sentMessages;

    void send(const std::string& msg) {
        sentMessages.push_back(msg);
    }

    Room* getCurrentRoom() { return nullptr; }
    const Room* getCurrentRoom() const { return nullptr; }
    void setCurrentRoom(const std::string& room_id) {}
    uint32_t getId() const { return 1; }

private:
    Player player_;
};

// 将MockSession转换为Session指针的适配器
class SessionAdapter : public Session {
public:
    SessionAdapter(MockSession* mock, MudServer& server)
        : Session(-1, 1, server), mock_(mock) {}

    void send(const std::string& message) override {
        if (mock_) {
            mock_->send(message);
        }
    }

private:
    MockSession* mock_;
};

class CommandRegistryTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 获取CommandRegistry实例（单例）
        registry_ = &CommandRegistry::getInstance();

        // 创建一个MudServer用于测试
        server_ = std::make_unique<MudServer>(9999);
        mock_session_ = std::make_unique<MockSession>();

        // 注册一些基础命令用于测试
        registry_->registerCommand(std::make_shared<HelpCommand>());
        registry_->registerCommand(std::make_shared<LookCommand>());
        registry_->registerCommand(std::make_shared<WhoCommand>());
        registry_->registerCommand(std::make_shared<SayCommand>());
    }

    void TearDown() override {
        // 不删除单例，保持全局状态
    }

    CommandRegistry* registry_;
    std::unique_ptr<MudServer> server_;
    std::unique_ptr<MockSession> mock_session_;
};

// 测试命令注册
TEST_F(CommandRegistryTest, RegisterCommand) {
    auto command = std::make_shared<MoveCommand>();
    std::string cmdName = command->getName();

    EXPECT_FALSE(registry_->hasCommand(cmdName));

    registry_->registerCommand(command);
    EXPECT_TRUE(registry_->hasCommand(cmdName));
}

// 测试命令注册大小写不敏感
TEST_F(CommandRegistryTest, CaseInsensitiveCommand) {
    auto command = std::make_shared<MoveCommand>();

    registry_->registerCommand(command);

    // 应该能识别各种大小写形式
    EXPECT_TRUE(registry_->hasCommand("move"));
    EXPECT_TRUE(registry_->hasCommand("Move"));
    EXPECT_TRUE(registry_->hasCommand("MOVE"));
    EXPECT_TRUE(registry_->hasCommand("mOvE"));
}

// 测试命令带别名注册
TEST_F(CommandRegistryTest, RegisterCommandWithAlias) {
    auto command = std::make_shared<MoveCommand>();
    std::vector<std::string> aliases = {"go", "walk"};

    registry_->registerCommandWithAlias(command, aliases);

    // 主命令应该存在
    EXPECT_TRUE(registry_->hasCommand("move"));

    // 别名也应该存在
    EXPECT_TRUE(registry_->hasCommand("go"));
    EXPECT_TRUE(registry_->hasCommand("walk"));
}

// 测试执行命令
TEST_F(CommandRegistryTest, ExecuteCommand) {
    // 创建一个模拟的Session
    SessionAdapter adapter(mock_session_.get(), *server_);

    // 执行help命令
    std::string result = registry_->executeCommand(
        reinterpret_cast<Session*>(&adapter), "help");

    // help命令应该返回帮助信息
    EXPECT_FALSE(result.empty());
    EXPECT_NE(result.find("Command Help"), std::string::npos);
}

// 测试执行不存在的命令
TEST_F(CommandRegistryTest, ExecuteUnknownCommand) {
    SessionAdapter adapter(mock_session_.get(), *server_);

    std::string result = registry_->executeCommand(
        reinterpret_cast<Session*>(&adapter), "nonexistent");

    // 应该返回错误信息
    EXPECT_FALSE(result.empty());
    EXPECT_NE(result.find("Unknown command"), std::string::npos);
}

// 测试命令带参数执行
TEST_F(CommandRegistryTest, ExecuteCommandWithArgs) {
    SessionAdapter adapter(mock_session_.get(), *server_);

    // 执行say命令带参数
    std::string result = registry_->executeCommand(
        reinterpret_cast<Session*>(&adapter), "say Hello World");

    // say命令应该处理参数
    EXPECT_FALSE(result.empty());
}

// 测试获取所有帮助信息
TEST_F(CommandRegistryTest, GetAllHelp) {
    std::string help = registry_->getAllHelp();

    // 验证帮助信息包含已知命令
    EXPECT_NE(help.find("help"), std::string::npos);
    EXPECT_NE(help.find("look"), std::string::npos);
    EXPECT_NE(help.find("who"), std::string::npos);
    EXPECT_NE(help.find("say"), std::string::npos);
}

// 测试空输入
TEST_F(CommandRegistryTest, EmptyInput) {
    SessionAdapter adapter(mock_session_.get(), *server_);

    std::string result = registry_->executeCommand(
        reinterpret_cast<Session*>(&adapter), "");

    // 空输入应该返回空字符串
    EXPECT_TRUE(result.empty());
}

// 测试空白输入
TEST_F(CommandRegistryTest, WhitespaceInput) {
    SessionAdapter adapter(mock_session_.get(), *server_);

    std::string result = registry_->executeCommand(
        reinterpret_cast<Session*>(&adapter), "   ");

    // 空白输入应该返回空字符串
    EXPECT_TRUE(result.empty());
}

// 测试单例模式
TEST_F(CommandRegistryTest, SingletonInstance) {
    CommandRegistry& instance1 = CommandRegistry::getInstance();
    CommandRegistry& instance2 = CommandRegistry::getInstance();

    EXPECT_EQ(&instance1, &instance2);
}

// 测试命令覆盖
TEST_F(CommandRegistryTest, CommandOverride) {
    // 第一次注册
    auto command1 = std::make_shared<HelpCommand>();
    registry_->registerCommand(command1);
    EXPECT_TRUE(registry_->hasCommand("help"));

    // 第二次注册（应该覆盖）
    auto command2 = std::make_shared<HelpCommand>();
    registry_->registerCommand(command2);
    EXPECT_TRUE(registry_->hasCommand("help"));

    // 执行命令应该使用新的实现
    SessionAdapter adapter(mock_session_.get(), *server_);
    std::string result = registry_->executeCommand(
        reinterpret_cast<Session*>(&adapter), "help");

    EXPECT_FALSE(result.empty());
}

// 测试多单词命令输入
TEST_F(CommandRegistryTest, MultiWordCommand) {
    SessionAdapter adapter(mock_session_.get(), *server_);

    // 测试多单词的say命令
    std::string result = registry_->executeCommand(
        reinterpret_cast<Session*>(&adapter), "say This is a test message");

    EXPECT_FALSE(result.empty());
}

// 测试命令参数解析
TEST_F(CommandRegistryTest, CommandArgumentParsing) {
    SessionAdapter adapter(mock_session_.get(), *server_);

    // 测试参数被正确解析
    std::string result1 = registry_->executeCommand(
        reinterpret_cast<Session*>(&adapter), "say hello");
    std::string result2 = registry_->executeCommand(
        reinterpret_cast<Session*>(&adapter), "say hello world");

    // 两个结果都应该包含say命令的输出
    EXPECT_FALSE(result1.empty());
    EXPECT_FALSE(result2.empty());
}

// 测试命令名称标准化
TEST_F(CommandRegistryTest, CommandNameNormalization) {
    // 测试命令名被转换为小写
    EXPECT_TRUE(registry_->hasCommand("HELP"));
    EXPECT_TRUE(registry_->hasCommand("Help"));
    EXPECT_TRUE(registry_->hasCommand("help"));
}

// 测试获取命令帮助的格式
TEST_F(CommandRegistryTest, HelpFormat) {
    std::string help = registry_->getAllHelp();

    // 验证帮助格式包含必要的章节
    EXPECT_NE(help.find("Basic Commands"), std::string::npos);
    EXPECT_NE(help.find("Movement Commands"), std::string::npos);
    EXPECT_NE(help.find("Combat Commands"), std::string::npos);
}

// 测试命令存在性检查
TEST_F(CommandRegistryTest, HasCommand) {
    EXPECT_TRUE(registry_->hasCommand("help"));
    EXPECT_TRUE(registry_->hasCommand("look"));
    EXPECT_TRUE(registry_->hasCommand("who"));
    EXPECT_FALSE(registry_->hasCommand("nonexistent"));
    EXPECT_FALSE(registry_->hasCommand(""));
}

} // namespace mud
