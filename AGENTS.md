# AGENTS.md - MudGame AI 助手指南

## 项目概述
**C++17 MUD 游戏** | CMake 3.15+ | Google Test 1.14.0 | TCP 服务器/客户端

## 目录结构
```
MudGame/
├── CMakeLists.txt          # 构建配置
├── include/                # 头文件
│   ├── combat/             # Combat.h, Monster.h
│   ├── command/            # ICommand.h, CommandRegistry.h (17 命令)
│   ├── connection/         # Connection.h
│   ├── event/              # EventBus.h, Events.h
│   ├── inventory/          # Inventory.h
│   ├── player/             # Player.h
│   ├── save/               # SaveManager.h
│   ├── server/             # MudServer.h, Session.h
│   ├── shop/               # Shop.h (物品交易)
│   ├── strategy/           # DamageStrategy.h
│   └── world/              # Room.h
├── src/                    # 源文件 (与 include 对应)
├── tests/                  # 单元测试 (150+ 用例)
└── docs/                   # 设计文档
```

## 核心模块
| 模块 | 关键类 | 职责 |
|------|--------|------|
| 服务器 | `MudServer`, `Session` | TCP 连接、会话管理 |
| 命令 | `ICommand`, `CommandRegistry` | 命令注册/执行、别名支持 |
| 战斗 | `CombatManager`, `Monster` | 战斗状态机、5 种怪物 |
| 策略 | `DamageStrategy` | 物理/魔法/元素伤害计算 |
| 玩家 | `Player`, `Inventory` | 属性成长、背包管理 |
| 商店 | `Shop`, `TradeResult` | 物品买卖、价格修改器 |
| 事件 | `EventBus` | 发布/订阅、模块解耦 |

## 编码规范
```cpp
// 命名：类/函数 PascalCase，成员变量 snake_case_
class DamageStrategy { int damage_value_; };

// 头文件防护 + 命名空间
#ifndef HEADER_H
#define HEADER_H
namespace mud { ... }
#endif

// 智能指针优先
auto player = std::make_unique<Player>("Hero");

// const 正确性
const std::string& getName() const { return name_; }

// 测试命名
TEST_F(ShopTest, BuyItem_Success) { ... }
```

## 设计模式
- **命令模式**: 玩家指令继承 `ICommand`
- **观察者模式**: 模块通信通过 `EventBus`
- **策略模式**: 伤害计算使用 `DamageStrategy`
- **单例模式**: `CommandRegistry`, `EventBus`, `SaveManager`

## 禁止操作
| ❌ | ✅ |
|---|---|
| `new`/`delete` | `std::make_unique` |
| 模块直接调用 | `EventBus` 发布 |
| 忽略返回值 | 检查 `TradeResult` |
| 头文件 `using namespace` | 源文件中使用 |
| 单例修改不加锁 | `std::atomic` |

## 快速开始
```bash
mkdir build && cd build && cmake .. -DMUD_ENABLE_TESTS=ON
cmake --build . --config Release && ctest --output-on-failure
```

## 添加功能
| 功能 | 步骤 |
|------|------|
| 新命令 | 继承 `ICommand` → `registerCommand()` |
| 新怪物 | `Monster::createXXX()` |
| 新物品 | `Shop::addItem()` |
| 新事件 | `Events.h` 添加 → `EventBus::publish()` |
| 新测试 | `tests/test_xxx.cpp` + `TEST_F` |

## 测试要求
- 核心 API 覆盖率 ≥ 80%
- 边界条件 + 失败路径必须测试

## 修改检查清单
- [ ] 头文件 `#ifndef` 防护
- [ ] `mud` 命名空间
- [ ] 成员变量 `_` 后缀
- [ ] 添加对应测试
- [ ] 更新 `CMakeLists.txt`
- [ ] 运行 `ctest`
