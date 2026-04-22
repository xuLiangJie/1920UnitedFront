# 新增单元测试指南

## 📋 概述

本次迭代为以下核心模块补充了单元测试，确保重构前后行为一致：

1. **EventBus（事件总线）** - `test_event_bus.cpp`
2. **CommandRegistry（命令注册中心）** - `test_command_registry_extended.cpp`
3. **CombatManager（战斗管理器）** - `test_combat_manager.cpp`

## 🎯 测试覆盖范围

### 1. EventBus 测试 (test_event_bus.cpp)

**测试用例数量**: 11个

**测试内容**:
- ✅ 基本事件发布和订阅
- ✅ 多个订阅者接收同一事件
- ✅ 不同事件类型的独立处理
- ✅ 多次发布事件
- ✅ 取消订阅功能
- ✅ 玩家攻击事件
- ✅ 怪物攻击事件
- ✅ 玩家死亡事件
- ✅ 战斗结束事件的完整数据
- ✅ 单例模式验证
- ✅ 未订阅事件类型不崩溃

**验证目标**:
- 确保事件总线正确发布和订阅事件
- 验证事件数据完整性
- 确保多订阅者机制正常工作
- 验证重构后的事件驱动架构

### 2. CommandRegistry 测试 (test_command_registry_extended.cpp)

**测试用例数量**: 15个

**测试内容**:
- ✅ 命令注册功能
- ✅ 命令名称大小写不敏感
- ✅ 命令带别名注册
- ✅ 执行命令
- ✅ 执行不存在的命令
- ✅ 命令带参数执行
- ✅ 获取所有帮助信息
- ✅ 空输入处理
- ✅ 空白输入处理
- ✅ 单例模式验证
- ✅ 命令覆盖
- ✅ 多单词命令输入
- ✅ 命令参数解析
- ✅ 命令名称标准化
- ✅ 帮助格式验证
- ✅ 命令存在性检查

**验证目标**:
- 确保命令注册中心正确管理所有命令
- 验证命令执行机制正常
- 确保命令别名功能正常
- 验证重构后的命令模式实现

### 3. CombatManager 测试 (test_combat_manager.cpp)

**测试用例数量**: 14个

**测试内容**:
- ✅ 初始战斗状态
- ✅ 开始战斗
- ✅ 玩家攻击
- ✅ 怪物攻击
- ✅ 战斗胜利
- ✅ 战斗失败
- ✅ 战斗状态描述
- ✅ 不在战斗中攻击
- ✅ 结束战斗
- ✅ 连续攻击直到胜利
- ✅ 不同怪物类型
- ✅ 暴击机制
- ✅ 防御减免
- ✅ 战斗状态机转换

**验证目标**:
- 确保战斗管理器正确管理战斗状态
- 验证事件驱动的战斗流程
- 确保攻击、伤害、胜利/失败逻辑正确
- 验证重构后与 Session 的解耦

## 🚀 运行测试

### 方法1: 使用提供的脚本（推荐）

```bash
# Windows
test_new_tests.bat

# 这会：
# 1. 配置 CMake（启用测试）
# 2. 编译项目
# 3. 运行新的测试
# 4. 运行完整测试套件
```

### 方法2: 手动运行

```bash
# 1. 创建构建目录
mkdir build
cd build

# 2. 配置 CMake（启用测试）
cmake .. -DMUD_ENABLE_TESTS=ON

# 3. 编译
cmake --build . --config Release --parallel

# 4. 运行特定测试模块

# 运行所有 EventBus 测试
./mud_tests --gtest_filter="EventBusTest.*"

# 运行所有 CommandRegistry 测试
./mud_tests --gtest_filter="CommandRegistryTest.*"

# 运行所有 CombatManager 测试
./mud_tests --gtest_filter="CombatManagerTest.*"

# 5. 运行完整测试套件
./mud_tests
# 或使用 CTest
ctest --output-on-failure
```

### 方法3: 在 CI 环境中运行

测试已自动纳入 CI 流水线，会在以下情况自动运行：
- Push 到 `develop`, `main`, `master` 分支
- Pull Request 到上述分支
- 手动触发 GitHub Actions

## 📊 测试统计

| 模块 | 测试文件 | 测试用例数 | 覆盖功能 |
|------|---------|-----------|---------|
| EventBus | test_event_bus.cpp | 11 | 事件发布/订阅、多订阅者、事件类型 |
| CommandRegistry | test_command_registry_extended.cpp | 15 | 命令注册、执行、别名、参数处理 |
| CombatManager | test_combat_manager.cpp | 14 | 战斗状态、攻击、胜利/失败、事件 |
| **总计** | **3个文件** | **40个用例** | **核心重构模块** |

## 🔍 重构前后行为验证

### EventBus（新模块）
- **重构前**: 无事件系统，模块间直接调用
- **重构后**: 事件总线实现
- **验证**: 通过 11 个测试确保事件机制正确

### CommandRegistry（重构）
- **重构前**: CommandHandler 600+ 行，所有命令耦合
- **重构后**: CommandRegistry + 17 个独立命令类
- **验证**: 通过 15 个测试确保命令注册和执行一致

### CombatManager（重构）
- **重构前**: 直接依赖 Session，紧耦合
- **重构后**: 通过 EventBus 发布事件，完全解耦
- **验证**: 通过 14 个测试确保战斗逻辑不变

## 🎨 测试设计原则

1. **独立性**: 每个测试用例独立运行，不依赖其他测试
2. **可重复性**: 测试结果可重复，不受外部因素影响
3. **快速反馈**: 测试执行时间短，适合频繁运行
4. **清晰命名**: 测试名称清楚描述测试内容
5. **全面覆盖**: 覆盖正常流程、边界情况、异常情况

## 🐛 调试测试

如果测试失败，可以：

1. **运行详细输出**:
```bash
./mud_tests --gtest_filter="EventBusTest.BasicPublishSubscribe" --gtest_break_on_failure
```

2. **查看详细日志**:
```bash
./mud_tests --gtest_filter="EventBusTest.*" --gtest_print_time=1
```

3. **只运行失败的测试**:
```bash
./mud_tests --gtest_filter="EventBusTest.FailingTest:*"
```

## 📈 CI 集成

新测试已完全集成到 CI 流水线：

```yaml
# .github/workflows/ci.yml
- name: Build and Test
  run: |
    cmake -B build -DMUD_ENABLE_TESTS=ON
    cmake --build build --parallel
    cd build
    ctest --output-on-failure
```

CI 会：
- ✅ 在多个平台编译（Windows MSVC, Ubuntu GCC/Clang）
- ✅ 自动运行所有单元测试
- ✅ 测试失败时阻止 PR 合并
- ✅ 上传测试结果和构建产物

## 🔄 持续维护

### 添加新测试

当添加新功能时，应该：

1. 在对应的测试文件中添加测试用例
2. 确保测试覆盖新功能的正常和异常情况
3. 运行测试验证不破坏现有功能
4. 提交 PR 时 CI 会自动验证

### 更新测试

当重构代码时：

1. 先运行现有测试，确保全部通过
2. 如果测试失败，更新测试以匹配新的行为
3. 添加新的测试用例覆盖重构的变化
4. 确保测试覆盖率不降低

## 📝 总结

本次新增的 40 个单元测试全面覆盖了本次迭代的核心模块：

- ✅ **EventBus**: 11 个测试，验证事件系统功能
- ✅ **CommandRegistry**: 15 个测试，验证命令注册和执行
- ✅ **CombatManager**: 14 个测试，验证战斗系统逻辑

这些测试确保了重构前后行为一致，并且已经完全集成到 CI 流水线中，每次代码提交都会自动运行。

---

**文档版本**: 1.0
**创建日期**: 2026-04-22
**维护者**: Development Team
