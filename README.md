# 西方奇幻 MUD 游戏

一个基于 TCP 的多用户地牢（MUD）游戏，使用 C++ 和 Boost.ASIO 编写。

## 功能特性

- TCP 服务器/客户端架构
- 多玩家支持
- 实时命令处理
- 西方奇幻主题场景
- 玩家间交流系统

## 依赖

- C++17 或更高版本
- CMake 3.15+
- Boost 库（特别是 Boost.ASIO）

## 编译

```bash
# 创建构建目录
mkdir build
cd build

# 配置项目
cmake ..

# 编译
cmake --build . --config Release
```

## 运行

### 启动服务器

```bash
# 默认端口 8888
./mud_server

# 或指定端口
./mud_server 9000
```

### 启动客户端

```bash
# 连接本地服务器
./mud_client

# 或指定服务器地址和端口
./mud_client 127.0.0.1 8888
```

## 可用命令

### 基本命令
| 命令 | 描述 |
|------|------|
| `help` | 显示帮助信息 |
| `who` | 查看在线玩家 |
| `look` | 查看周围环境 |
| `score` | 查看角色状态 |
| `name <名字>` | 设置角色名字 |
| `say <话语>` | 说话 |

### 移动命令
| 命令 | 描述 |
|------|------|
| `north` / `n` | 向北移动 |
| `south` / `s` | 向南移动 |
| `east` / `e` | 向东移动 |
| `west` / `w` | 向西移动 |

### 其他命令
| 命令 | 描述 |
|------|------|
| `inventory` / `i` | 查看物品栏 |
| `quit` | 退出游戏 |

## 快速开始

1. 启动服务器：`./mud_server`
2. 启动客户端：`./mud_client`
3. 设置名字：`name 英雄`
4. 查看帮助：`help`
5. 探索世界：`look`, `north`, `south` 等

## 项目结构

```
MudGame/
├── CMakeLists.txt
├── include/
│   ├── server/
│   │   ├── MudServer.h
│   │   └── Session.h
│   ├── command/
│   │   └── CommandHandler.h
│   ├── player/
│   │   └── Player.h
│   └── client/
│       └── MudClient.h
└── src/
    ├── main_server.cpp
    ├── main_client.cpp
    ├── server/
    │   ├── MudServer.cpp
    │   └── Session.cpp
    ├── command/
    │   └── CommandHandler.cpp
    ├── player/
    │   └── Player.cpp
    └── client/
        └── MudClient.cpp
```

## 示例会话

```
╔═══════════════════════════════════════════════════════════╗
║                                                       ║
║           欢迎来到西方奇幻 MUD 游戏！                   ║
║                                                       ║
╚═══════════════════════════════════════════════════════════╝

你是一位踏入这个神秘世界的冒险者。在这里，你可以探索未知的地域，
与其他冒险者交流，体验奇妙的奇幻之旅。

输入 'help' 查看可用命令，'name <名字>' 来设置你的角色名字。

> name 亚瑟
你的名字已从 "无名者" 改为 "亚瑟"。

> look
=== 起始大厅 ===
你站在一个宏伟的石头大厅中央。墙壁上挂着古老的旗帜，
描绘着英勇的骑士与恶龙战斗的场景...

> who
=== 在线玩家 ===
  - 亚瑟
==================
```
"# MUD Game" 
