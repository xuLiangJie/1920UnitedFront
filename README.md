# Western Fantasy MUD Game

一个基于 TCP 的多用户地牢（MUD）游戏，采用 C++17 编写，使用多种设计模式实现高内聚、低耦合的架构。

## 📋 目录

- [项目概述](#项目概述)
- [系统架构](#系统架构)
- [核心模块](#核心模块)
- [环境搭建](#环境搭建)
- [编译构建](#编译构建)
- [运行测试](#运行测试)
- [游戏玩法](#游戏玩法)
- [开发指南](#开发指南)
- [项目结构](#项目结构)
- [常见问题](#常见问题)
- [贡献指南](#贡献指南)

---

## 项目概述

### 功能特性

- 🌐 **TCP 服务器/客户端架构** - 支持多玩家同时在线
- ⚔️ **完整的战斗系统** - 回合制战斗，支持暴击、防御减免
- 🗺️ **西方奇幻世界** - 5个预设房间，丰富的NPC和怪物
- 💾 **存档系统** - JSON格式存档，支持多角色
- 🎯 **命令系统** - 17个游戏命令，支持别名
- 📦 **背包系统** - 物品管理，支持排序和查找
- 🔄 **事件驱动** - 基于事件总线的模块解耦
- 🧪 **完善测试** - 100+ 单元测试，CI/CD 自动化

### 技术栈

| 技术 | 版本 | 用途 |
|------|------|------|
| C++ | 17+ | 核心开发语言 |
| CMake | 3.15+ | 构建系统 |
| Google Test | 1.14.0 | 单元测试框架 |
| 标准 C++ 库 | - | 网络、线程、文件IO |

---

## 系统架构

### 架构图

```
┌─────────────────────────────────────────────────────────────────┐
│                        客户端层 (Client)                       │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐         │
│  │ MudClient    │  │  Connection  │  │   Network    │         │
│  │  (UI/输入)   │  │  (连接管理)  │  │  (Socket)    │         │
│  └──────────────┘  └──────────────┘  └──────────────┘         │
└───────────────────────────┬─────────────────────────────────────┘
                            │ TCP 连接
┌───────────────────────────┴─────────────────────────────────────┐
│                      服务器层 (Server)                          │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                    MudServer                            │   │
│  │              (会话管理、广播、世界管理)                  │   │
│  └───────────────┬─────────────────────────────────────────┘   │
│                  │                                               │
│  ┌───────────────┴─────────────────────────────────────────┐   │
│  │                    Session (会话)                        │   │
│  │  ┌─────────┐  ┌──────────┐  ┌──────────────┐           │   │
│  │  │ Player  │  │CombatMgr │  │   EventBus   │           │   │
│  │  │(玩家数据)│  │(战斗管理)│  │  (事件订阅)  │           │   │
│  │  └─────────┘  └──────────┘  └──────────────┘           │   │
│  └─────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
                            │
┌───────────────────────────┴─────────────────────────────────────┐
│                      业务逻辑层 (Business Logic)                 │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐         │
│  │ CommandReg   │  │    World     │  │  SaveManager │         │
│  │  (命令注册)  │  │  (世界地图)  │  │  (存档管理)  │         │
│  └──────────────┘  └──────────────┘  └──────────────┘         │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐         │
│  │ 17 Commands  │  │   Inventory  │  │   Events     │         │
│  │  (独立命令)  │  │  (背包系统)  │  │  (事件定义)  │         │
│  └──────────────┘  └──────────────┘  └──────────────┘         │
└─────────────────────────────────────────────────────────────────┘
                            │
┌───────────────────────────┴─────────────────────────────────────┐
│                      数据层 (Data)                              │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐         │
│  │    Player    │  │   Monster    │  │     Room     │         │
│  │  (玩家数据)  │  │  (怪物数据)  │  │  (房间数据)  │         │
│  └──────────────┘  └──────────────┘  └──────────────┘         │
│  ┌──────────────┐  ┌──────────────┐                           │
│  │     NPC      │  │     Item     │                           │
│  │  (NPC数据)   │  │  (物品数据)  │                           │
│  └──────────────┘  └──────────────┘                           │
└─────────────────────────────────────────────────────────────────┘
```

### 模块依赖关系

```
MudServer
    ├── Session
    │   ├── Player
    │   ├── CombatManager
    │   └── EventBus (订阅)
    ├── World
    │   ├── Room
    │   ├── Monster
    │   └── NPC
    ├── CommandRegistry
    │   └── ICommand (17个实现)
    └── SaveManager
        ├── PlayerData
        └── Item

CombatManager
    ├── Monster
    ├── EventBus (发布)
    └── DamageStrategy

Connection
    └── Network
```

---

## 核心模块

### 1. 服务器核心 (Server Core)

#### MudServer
**职责**: 游戏服务器主控类
- 管理所有玩家会话
- 处理TCP连接和断开
- 消息广播（全局和房间内）
- 世界地图管理

**关键文件**:
- `include/server/MudServer.h`
- `src/server/MudServer.cpp`

#### Session
**职责**: 单个玩家会话管理
- 管理玩家连接生命周期
- 处理玩家输入和输出
- 订阅游戏事件
- 维护玩家当前房间

**关键文件**:
- `include/server/Session.h`
- `src/server/Session.cpp`

#### Connection
**职责**: 网络连接管理
- 异步消息接收和发送
- 消息队列管理
- 线程安全的网络I/O

**关键文件**:
- `include/connection/Connection.h`
- `src/connection/Connection.cpp`

### 2. 命令系统 (Command System)

#### CommandRegistry
**职责**: 命令注册和路由
- 管理所有命令的注册
- 命令名称大小写标准化
- 命令执行和路由
- 帮助信息生成

**关键文件**:
- `include/command/CommandRegistry.h`
- `src/command/CommandRegistry.cpp`

#### 命令接口和实现
**职责**: 具体游戏命令实现
- 17个独立命令类
- 每个命令单一职责
- 支持命令别名

**命令列表**:
- HelpCommand, LookCommand, WhoCommand, ScoreCommand
- NameCommand, SayCommand, QuitCommand
- InventoryCommand, MoveCommand, KillCommand
- AttackCommand, FleeCommand
- SaveCommand, LoadCommand, DeleteCommand, SavesCommand

**关键文件**:
- `include/command/ICommand.h` (命令接口)
- `include/command/*.h` (具体命令)
- `src/command/*.cpp` (命令实现)

### 3. 战斗系统 (Combat System)

#### CombatManager
**职责**: 战斗流程管理
- 战斗状态机（未战斗/战斗中/胜利/失败）
- 玩家攻击处理
- 怪物攻击处理
- 战斗结果计算
- 通过事件总线发布战斗事件

**关键文件**:
- `include/combat/Combat.h`
- `src/combat/Combat.cpp`

#### Monster
**职责**: 怪物数据和行为
- 怪物属性（HP、攻击、防御、经验奖励）
- 怪物难度分级
- 预设怪物工厂方法
- 攻击和受伤行为

**关键文件**:
- `include/combat/Monster.h`
- `src/combat/Monster.cpp`

#### DamageStrategy
**职责**: 伤害计算策略
- 策略模式实现
- 物理伤害计算
- 暴击机制
- 防御减免

**关键文件**:
- `include/strategy/DamageStrategy.h`
- `src/strategy/DamageStrategy.cpp`

### 4. 世界系统 (World System)

#### World
**职责**: 游戏世界管理
- 房间注册和查找
- 房间导航
- 预设世界生成

**关键文件**:
- `include/world/Room.h`
- `src/world/Room.cpp`

#### Room
**职责**: 单个房间管理
- 房间基本信息（名称、描述）
- 出口管理（8个方向）
- NPC 管理
- 怪物管理

**关键文件**:
- `include/world/Room.h`
- `src/world/Room.cpp`

### 5. 玩家系统 (Player System)

#### Player
**职责**: 玩家数据和行为
- 角色属性（等级、经验、HP、MP）
- 战斗属性（攻击、防御）
- 背包管理
- 升级系统

**关键文件**:
- `include/player/Player.h`
- `src/player/Player.cpp`

#### Inventory
**职责**: 背包系统
- 物品添加/移除
- 物品查找
- 价值计算
- 排序功能

**关键文件**:
- `include/inventory/Inventory.h`
- `src/inventory/Inventory.cpp`

### 6. 存档系统 (Save System)

#### SaveManager
**职责**: 存档管理
- JSON 格式存档
- 保存/加载玩家数据
- 存档列表管理
- 存档删除

**关键文件**:
- `include/save/SaveManager.h`
- `src/save/SaveManager.cpp`

### 7. 事件系统 (Event System)

#### EventBus
**职责**: 事件发布和订阅
- 观察者模式实现
- 事件类型管理
- 多订阅者支持
- 线程安全

**关键文件**:
- `include/event/EventBus.h`
- `src/event/EventBus.cpp`

#### Events
**职责**: 事件定义
- 事件类型枚举
- 具体事件类
- 事件数据结构

**关键文件**:
- `include/event/Events.h`

---

## 环境搭建

### 系统要求

| 组件 | 最低版本 | 推荐版本 |
|------|---------|---------|
| 操作系统 | Windows 10+, Ubuntu 20.04+ | 最新版本 |
| 编译器 | GCC 7+, Clang 5+, MSVC 19.20+ | GCC 11+, Clang 14+, MSVC 19.35+ |
| CMake | 3.15+ | 3.25+ |
| C++ 标准 | C++17 | C++17/20 |
| 内存 | 2GB | 4GB+ |
| 磁盘空间 | 500MB | 1GB+ |

### Windows 环境

#### 1. 安装 Visual Studio

```powershell
# 下载并安装 Visual Studio Community (免费)
# https://visualstudio.microsoft.com/downloads/

# 安装时选择 "使用 C++ 的桌面开发" 工作负载
# 必选组件：
#   - MSVC v143 - VS 2022 C++ x64/x86 生成工具
#   - Windows 10 SDK (或更高版本)
```

#### 2. 安装 CMake

```powershell
# 使用 Chocolatey 安装
choco install cmake

# 或从官网下载安装
# https://cmake.org/download/

# 验证安装
cmake --version
```

#### 3. 安装 Git（可选）

```powershell
# 使用 Chocolatey 安装
choco install git

# 或从官网下载安装
# https://git-scm.com/download/win

# 验证安装
git --version
```

#### 4. 克隆项目

```powershell
# 克隆仓库
git clone https://github.com/your-username/MudGame.git
cd MudGame
```

### Linux/macOS 环境

#### 1. 安装编译工具

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install -y build-essential cmake git

# macOS (使用 Homebrew)
brew install cmake git

# 验证安装
gcc --version
cmake --version
git --version
```

#### 2. 克隆项目

```bash
# 克隆仓库
git clone https://github.com/your-username/MudGame.git
cd MudGame
```

### IDE 配置（推荐）

#### Visual Studio Code

1. 安装 VS Code: https://code.visualstudio.com/
2. 安装扩展：
   - C/C++ (Microsoft)
   - CMake Tools (Microsoft)
   - GitLens (GitKraken)

3. 打开项目文件夹
4. VS Code 会自动识别 CMake 项目

#### Visual Studio 2022

1. 打开 Visual Studio
2. 选择 "打开本地文件夹"
3. 选择项目根目录
4. Visual Studio 会自动配置 CMake 项目

---

## 编译构建

### 基本编译

```bash
# 创建构建目录
mkdir build
cd build

# 配置项目（Debug 模式）
cmake ..

# 编译项目
cmake --build . --config Debug

# 或 Release 模式
cmake --build . --config Release
```

### 启用单元测试

```bash
# 配置时启用测试
cmake .. -DMUD_ENABLE_TESTS=ON

# 编译
cmake --build . --config Release

# 运行测试
ctest --output-on-failure
```

### 高级编译选项

```bash
# 启用严格警告（警告视为错误）
cmake .. -DMUD_ENABLE_STRICT_WARNINGS=ON

# 指定编译器
cmake .. -DCMAKE_CXX_COMPILER=g++

# 指定生成器
cmake .. -G "Visual Studio 17 2022" -A x64
```

### 编译产物

编译成功后，在 `build/` 目录下生成：

| 文件 | 平台 | 说明 |
|------|------|------|
| `mud_server.exe` / `mud_server` | Windows/Linux | 服务器可执行文件 |
| `mud_client.exe` / `mud_client` | Windows/Linux | 客户端可执行文件 |
| `mud_tests.exe` / `mud_tests` | Windows/Linux | 测试可执行文件（需启用测试） |

---

## 运行测试

### 运行所有测试

```bash
cd build

# 使用 CTest
ctest --output-on-failure

# 或直接运行测试可执行文件
./mud_tests  # Linux/Mac
./mud_tests.exe  # Windows
```

### 运行特定测试模块

```bash
# 只运行 EventBus 测试
./mud_tests --gtest_filter="EventBusTest.*"

# 只运行 CommandRegistry 测试
./mud_tests --gtest_filter="CommandRegistryTest.*"

# 只运行 CombatManager 测试
./mud_tests --gtest_filter="CombatManagerTest.*"

# 运行单个测试用例
./mud_tests --gtest_filter="EventBusTest.BasicPublishSubscribe"
```

### 测试覆盖范围

| 模块 | 测试文件 | 用例数 | 覆盖内容 |
|------|---------|-------|---------|
| EventBus | test_event_bus.cpp | 11 | 事件发布/订阅、多订阅者 |
| CommandRegistry | test_command_registry_extended.cpp | 15 | 命令注册/执行、别名 |
| CombatManager | test_combat_manager.cpp | 14 | 战斗状态、攻击、事件 |
| Player | test_player.cpp | 16 | 玩家属性、升级、物品 |
| Monster | test_monster.cpp | 14 | 怪物创建、攻击、难度 |
| Room | test_room.cpp | 17 | 房间管理、出口、NPC |
| Inventory | test_inventory.cpp | 13 | 物品管理、排序、查找 |
| DamageStrategy | test_damage_strategy.cpp | 13 | 伤害计算、暴击、策略 |
| SaveManager | test_save_manager.cpp | 12 | 存档保存/加载、序列化 |
| Commands | test_commands.cpp | 3 | 命令接口、注册 |
| **总计** | **10个文件** | **128+** | **核心功能全覆盖** |

详见 [docs/NEW_TESTS_GUIDE.md](docs/NEW_TESTS_GUIDE.md)

---

## 游戏玩法

### 启动游戏

#### 终端 1: 启动服务器

```bash
cd build
./mud_server  # 或 mud_server.exe (Windows)

# 输出:
# +------------------------------------------+
# |      Western Fantasy MUD Game Server     |
# +------------------------------------------+
#
# Server started, listening on port: 8888
# World loaded with Grand Starting Hall as starting point.
# Press Ctrl+C to stop
```

#### 终端 2: 启动客户端

```bash
cd build
./mud_client  # 或 mud_client.exe (Windows)

# 输出:
# +------------------------------------------+
# |      Western Fantasy MUD Game Client     |
# +------------------------------------------+
#
# Connecting to 127.0.0.1:8888...
# Connected!

# +-------------------------------------------------------+
# |     Welcome to Western Fantasy MUD Game!              |
# +-------------------------------------------------------+
#
# You are an adventurer entering this mysterious world.
# Explore unknown lands, meet other adventurers,
# and experience a wonderful fantasy journey.
#
# Type 'help' to see available commands.
# Type 'name <yourname>' to set your character name.
#
# >
```

### 游戏命令

#### 基本命令

```bash
# 查看帮助
> help

# 查看在线玩家
> who

# 查看周围环境
> look

# 查看角色状态
> score

# 设置角色名称
> name Arthur

# 说话
> say Hello, everyone!
```

#### 移动命令

```bash
# 向北移动（也可使用 n）
> north

# 向南移动（也可使用 s）
> south

# 向东移动（也可使用 e）
> east

# 向西移动（也可使用 w）
> west

# 其他方向
> up / down / enter / exit
```

#### 战斗命令

```bash
# 发起战斗
> kill Goblin

# 继续攻击
> attack

# 逃跑
> flee
```

#### 存档命令

```bash
# 保存游戏
> save

# 加载存档
> load

# 查看存档列表
> saves

# 删除存档
> delete OldCharacter
```

#### 其他命令

```bash
# 查看背包
> inventory
> i

# 退出游戏（自动保存）
> quit
```

### 游戏世界

#### 房间列表

1. **Grand Starting Hall** (起始大厅)
   - 安全区域，无怪物
   - NPC: Elder Sage
   - 出口: north, east, west

2. **Crystal Corridor** (水晶走廊)
   - 怪物: Goblin (哥布林)
   - 出口: south, north

3. **Weapon Room** (武器室)
   - 怪物: Wolf (野狼)
   - NPC: Grom Ironhand
   - 出口: west

4. **Fountain Garden** (喷泉花园)
   - 怪物: Skeleton (骷髅)
   - NPC: Luna
   - 出口: east

5. **Mysterious Treasure Room** (神秘宝库)
   - 怪物: Orc (兽人 Boss)
   - 出口: south

#### 怪物列表

| 怪物 | 难度 | HP | 攻击 | 防御 | 经验奖励 |
|------|------|----|----|----|---------|
| Goblin | Easy | 30 | 8 | 2 | 10 |
| Wolf | Normal | 50 | 12 | 4 | 20 |
| Skeleton | Normal | 60 | 10 | 3 | 25 |
| Orc | Hard | 100 | 20 | 8 | 50 |
| Dragon | Boss | 200 | 30 | 12 | 100 |

---

## 开发指南

### 添加新命令

#### 1. 创建命令类

创建 `include/command/MyCommand.h`:

```cpp
#ifndef MY_COMMAND_H
#define MY_COMMAND_H

#include "command/ICommand.h"

namespace mud {

class MyCommand : public ICommand {
public:
    std::string execute(Session* session, const std::vector<std::string>& args) override {
        // 实现命令逻辑
        return "Command executed!\r\n";
    }

    std::string getHelp() const override {
        return "mycommand - Description of my command\r\n";
    }

    std::string getName() const override {
        return "mycommand";
    }
};

} // namespace mud

#endif // MY_COMMAND_H
```

创建 `src/command/MyCommand.cpp`:

```cpp
#include "command/MyCommand.h"
#include "server/Session.h"
#include <sstream>

namespace mud {

std::string MyCommand::execute(Session* session, const std::vector<std::string>& args) {
    std::ostringstream oss;

    if (args.empty()) {
        oss << "Usage: mycommand <arg>\r\n";
        return oss.str();
    }

    oss << "You executed mycommand with: " << args[0] << "\r\n";
    return oss.str();
}

} // namespace mud
```

#### 2. 注册命令

在 `src/command/CommandHandler.cpp` 的 `initializeCommands()` 中添加:

```cpp
#include "command/MyCommand.h"

void CommandHandler::initializeCommands() {
    // ... 其他命令注册

    // 注册新命令
    CommandRegistry::getInstance().registerCommand(std::make_shared<MyCommand>());

    // 如果需要别名
    CommandRegistry::getInstance().registerCommandWithAlias(
        std::make_shared<MyCommand>(),
        {"my", "cmd"}
    );
}
```

#### 3. 更新 CMakeLists.txt

在 `CMakeLists.txt` 中添加新源文件:

```cmake
add_executable(mud_server
    # ... 其他文件
    src/command/MyCommand.cpp
    # ...
)
```

#### 4. 重新编译和测试

```bash
cd build
cmake --build . --config Release
./mud_server
```

### 添加新事件

#### 1. 定义事件类型

在 `include/event/Events.h` 中添加:

```cpp
enum class EventType {
    // ... 现有事件
    MyNewEvent
};

struct MyNewEvent : public Event {
    uint32_t sessionId;
    std::string data;

    MyNewEvent(uint32_t id, const std::string& d)
        : sessionId(id), data(d) {}
};
```

#### 2. 发布事件

```cpp
#include "event/EventBus.h"
#include "event/Events.h"

// 发布事件
MyNewEvent event(sessionId, "event data");
EventBus::getInstance().publish(event);
```

#### 3. 订阅事件

```cpp
#include "event/EventBus.h"
#include "event/Events.h"

// 订阅事件
EventBus::getInstance().subscribe(EventType::MyNewEvent,
    [](const Event& e) {
        const auto& event = static_cast<const MyNewEvent&>(e);
        // 处理事件
    });
```

### 代码规范

#### 命名约定

| 类型 | 约定 | 示例 |
|------|------|------|
| 类名 | PascalCase | `class Player` |
| 函数名 | camelCase | `void sendMessage()` |
| 成员变量 | snake_case + 下划线后缀 | `int player_id_;` |
| 常量 | UPPER_SNAKE_CASE | `const int MAX_HP = 100;` |
| 文件名 | PascalCase | `Player.h`, `Player.cpp` |

#### 注释规范

```cpp
/**
 * @brief 玩家类，管理玩家数据和行为
 *
 * 此类负责管理玩家的所有属性和游戏行为，
 * 包括升级、背包、战斗属性等。
 */
class Player {
public:
    /**
     * @brief 添加经验值
     * @param amount 要添加的经验值数量
     * @note 当经验值足够时会触发升级
     */
    void addExp(int amount);

private:
    int level_;      ///< 玩家等级
    int exp_;        ///< 当前经验值
};
```

---

## 项目结构

```
MudGame/
├── CMakeLists.txt              # CMake 构建配置
├── README.md                   # 项目说明（本文件）
├── TEST_SUMMARY.md             # 测试补充总结
├── mud-game-protocol.yaml      # OpenAPI 接口契约
│
├── api/                        # API 文档
│   └── mud-game-protocol.yaml
│
├── build/                      # 构建输出目录（.gitignore）
│   ├── mud_server              # 服务器可执行文件
│   ├── mud_client              # 客户端可执行文件
│   └── mud_tests               # 测试可执行文件
│
├── docs/                       # 项目文档
│   ├── CI_CD_DEVELOPMENT.md    # CI/CD 开发规范
│   ├── NEW_TESTS_GUIDE.md      # 新增测试指南
│   ├── core_use_cases.md       # 核心用例文档
│   └── ...                     # 其他文档
│
├── include/                    # 头文件
│   ├── client/                 # 客户端
│   │   └── MudClient.h
│   ├── combat/                 # 战斗系统
│   │   ├── Combat.h
│   │   └── Monster.h
│   ├── command/                # 命令系统
│   │   ├── ICommand.h
│   │   ├── CommandRegistry.h
│   │   ├── CommandHandler.h
│   │   └── *.h                 # 17个具体命令
│   ├── connection/             # 网络连接
│   │   └── Connection.h
│   ├── event/                  # 事件系统
│   │   ├── EventBus.h
│   │   └── Events.h
│   ├── inventory/              # 背包系统
│   │   └── Inventory.h
│   ├── network/                # 网络层
│   │   └── Network.h
│   ├── player/                 # 玩家系统
│   │   └── Player.h
│   ├── save/                   # 存档系统
│   │   └── SaveManager.h
│   ├── server/                 # 服务器核心
│   │   ├── MudServer.h
│   │   └── Session.h
│   ├── strategy/               # 策略模式
│   │   └── DamageStrategy.h
│   └── world/                  # 世界系统
│       └── Room.h
│
├── src/                        # 源文件
│   ├── main_server.cpp         # 服务器入口
│   ├── main_client.cpp         # 客户端入口
│   ├── client/
│   │   └── MudClient.cpp
│   ├── combat/
│   │   ├── Combat.cpp
│   │   └── Monster.cpp
│   ├── command/
│   │   ├── CommandRegistry.cpp
│   │   ├── CommandHandler.cpp
│   │   └── *.cpp               # 17个具体命令实现
│   ├── connection/
│   │   └── Connection.cpp
│   ├── event/
│   │   └── EventBus.cpp
│   ├── inventory/
│   │   └── Inventory.cpp
│   ├── network/
│   │   └── Network.cpp
│   ├── player/
│   │   └── Player.cpp
│   ├── save/
│   │   └── SaveManager.cpp
│   ├── server/
│   │   ├── MudServer.cpp
│   │   └── Session.cpp
│   ├── strategy/
│   │   └── DamageStrategy.cpp
│   └── world/
│       └── Room.cpp
│
├── tests/                      # 单元测试
│   ├── test_player.cpp
│   ├── test_monster.cpp
│   ├── test_room.cpp
│   ├── test_inventory.cpp
│   ├── test_damage_strategy.cpp
│   ├── test_save_manager.cpp
│   ├── test_commands.cpp
│   ├── test_event_bus.cpp
│   ├── test_command_registry_extended.cpp
│   └── test_combat_manager.cpp
│
├── .github/                    # GitHub 配置
│   └── workflows/
│       └── ci.yml             # CI/CD 配置
│
├── .gitignore                  # Git 忽略规则
├── build_with_tests.bat        # Windows 测试构建脚本
└── validate_ci.bat             # CI 配置验证脚本
```

---

## 常见问题

### 编译问题

#### Q: CMake 找不到编译器

**A**: 确保已安装编译器并正确配置环境变量

```bash
# Windows: 确保 MSVC 在 PATH 中
# 或使用 Visual Studio 的 Developer Command Prompt

# Linux: 安装 build-essential
sudo apt install build-essential

# macOS: 安装 Xcode Command Line Tools
xcode-select --install
```

#### Q: 链接错误：找不到符号

**A**: 清理构建目录重新编译

```bash
rm -rf build
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### 运行问题

#### Q: 服务器启动失败

**A**: 检查端口是否被占用

```bash
# Linux/Mac
netstat -tuln | grep 8888
lsof -i :8888

# Windows
netstat -ano | findstr :8888
```

#### Q: 客户端无法连接服务器

**A**: 检查防火墙设置和网络连接

```bash
# 测试连接
telnet 127.0.0.1 8888

# 或使用 nc
nc -zv 127.0.0.1 8888
```

### 测试问题

#### Q: Google Test 下载失败

**A**: 手动下载或配置代理

```bash
# 设置 CMake 代理
cmake .. -DFETCHCONTENT_FULLY_DISCONNECTED=OFF

# 或手动下载 googletest 并指定路径
cmake .. -DFETCHCONTENT_SOURCE_DIR_GOOGLETEST=/path/to/googletest
```

---

## 贡献指南

### 代码提交规范

```bash
# 提交信息格式
<type>(<scope>): <subject>

<body>

<footer>
```

**类型 (type)**:
- `feat`: 新功能
- `fix`: 修复 Bug
- `docs`: 文档更新
- `style`: 代码格式（不影响功能）
- `refactor`: 重构
- `test`: 测试相关
- `chore`: 构建/工具相关

**示例**:
```bash
git commit -m "feat(combat): add critical hit system"
git commit -m "fix(player): resolve HP calculation bug"
git commit -m "docs(readme): update architecture diagram"
```

### Pull Request 流程

1. Fork 项目到自己的 GitHub 账号
2. 创建功能分支: `git checkout -b feature/my-feature`
3. 提交更改: `git commit -m 'feat: add my feature'`
4. 推送到分支: `git push origin feature/my-feature`
5. 创建 Pull Request

### PR 检查清单

- [ ] 代码通过所有单元测试
- [ ] 添加了必要的测试用例
- [ ] 更新了相关文档
- [ ] 遵循代码规范
- [ ] 提交信息清晰明确
- [ ] CI 流水线通过

---

## 许可证

本项目采用 MIT 许可证 - 详见 LICENSE 文件

---

## 联系方式

- **Issues**: [GitHub Issues](https://github.com/your-username/MudGame/issues)
- **Discussions**: [GitHub Discussions](https://github.com/your-username/MudGame/discussions)
- **Email**: your-email@example.com

---

## 致谢

感谢所有为这个项目做出贡献的开发者！

---

**最后更新**: 2026-04-22
**版本**: 1.0.0
**维护者**: Development Team
