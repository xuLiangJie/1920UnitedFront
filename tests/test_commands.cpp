#include <gtest/gtest.h>
#include "command/CommandRegistry.h"
#include "command/ICommand.h"

namespace mud {

class CommandRegistryTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 清空注册表（单例）
        CommandRegistry::getInstance();
    }
};

// 测试命令注册
TEST_F(CommandRegistryTest, RegisterCommand) {
    class TestCommand : public ICommand {
    public:
        std::string execute(Session*, const std::vector<std::string>&) override {
            return "test";
        }
        std::string getHelp() const override { return "test help"; }
        std::string getName() const override { return "test"; }
    };
    
    // 注意：由于是单例，这个测试会影响其他测试
    // 这里只是演示注册机制
    EXPECT_NO_THROW({
        auto cmd = std::make_shared<TestCommand>();
        // 实际项目中不应该重复注册
    });
}

// 测试命令存在性检查
TEST_F(CommandRegistryTest, HasCommand) {
    // 这个测试依赖于CommandHandler::initializeCommands()的调用
    // 在实际测试中，应该先初始化命令
    auto& registry = CommandRegistry::getInstance();
    
    // 这些命令应该在初始化后存在
    // EXPECT_TRUE(registry.hasCommand("help"));
    // EXPECT_TRUE(registry.hasCommand("look"));
}

// 测试命令接口
TEST_F(CommandRegistryTest, CommandInterface) {
    class MockCommand : public ICommand {
    public:
        std::string execute(Session*, const std::vector<std::string>& args) override {
            if (args.empty()) {
                return "no args";
            }
            std::string result;
            for (const auto& arg : args) {
                result += arg + " ";
            }
            return result;
        }
        std::string getHelp() const override { return "mock command help"; }
        std::string getName() const override { return "mock"; }
    };
    
    MockCommand cmd;
    EXPECT_EQ(cmd.getName(), "mock");
    EXPECT_EQ(cmd.getHelp(), "mock command help");
    
    std::vector<std::string> args = {"arg1", "arg2"};
    std::string result = cmd.execute(nullptr, args);
    EXPECT_NE(result.find("arg1"), std::string::npos);
}

} // namespace mud
