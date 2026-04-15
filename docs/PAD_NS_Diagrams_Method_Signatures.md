# MUD游戏核心控制类 PAD图与N-S盒图

## 📋 文档说明

本文档针对MUD游戏重构后的核心控制类,提供:
1. **PAD图** (Problem Analysis Diagram) - 树形结构展示程序控制流
2. **N-S盒图** (Nassi-Shneiderman) - 结构化流程图
3. **方法签名规范** - 跨类调用的接口细节

---

## 1. Session类 - 会话管理核心

### 1.1 Session::start() - 启动会话

#### PAD图

```
Session::start()
├─ 创建Connection对象
│  └─ connection_ = make_unique<Connection>(socket_fd_)
│
├─ 设置消息回调
│  └─ connection_->onMessage([this](msg) { onMessage(msg); })
│
├─ 设置关闭回调
│  └─ connection_->onClose([this]() { onClose(); })
│
├─ 启动网络连接
│  └─ connection_->start()
│
├─ 设置事件监听
│  └─ setupEventListeners()
│
└─ 发送欢迎消息
   ├─ send("\r\n+---...")
   ├─ send("| Welcome...")
   ├─ send("+---...")
   ├─ send("You are an adventurer...")
   ├─ send("Type 'help'...")
   └─ send("Type 'name'...")
```

#### N-S盒图

```
┌─────────────────────────────────────────────────────────────┐
│                    Session::start()                          │
├─────────────────────────────────────────────────────────────┤
│ connection_ = make_unique<Connection>(socket_fd_)            │
├─────────────────────────────────────────────────────────────┤
│ connection_->onMessage([this](msg) { onMessage(msg); })     │
├─────────────────────────────────────────────────────────────┤
│ connection_->onClose([this]() { onClose(); })               │
├─────────────────────────────────────────────────────────────┤
│ connection_->start()                                         │
├─────────────────────────────────────────────────────────────┤
│ setupEventListeners()                                        │
├─────────────────────────────────────────────────────────────┤
│ 发送欢迎消息序列 (10次send调用)                              │
└─────────────────────────────────────────────────────────────┘
```

**方法签名:**
```cpp
void Session::start() noexcept(false)
```
- **前置条件:** socket_fd_有效, server_引用有效
- **后置条件:** 连接已建立,事件监听已注册,欢迎消息已发送
- **异常:** 可能抛出std::bad_alloc(Connection创建失败)
- **线程安全:** 非线程安全,应在单线程中调用

---

### 1.2 Session::stop() - 停止会话

#### PAD图

```
Session::stop()
├─ 原子CAS操作
│  ├─ expected = false
│  ├─ if !stopped_.compare_exchange_strong(expected, true)
│  │  └─ return (防止重复停止)
│  └─ end if
│
├─ 停止连接
│  └─ if connection_ != nullptr
│     └─ connection_->stop()
│     └─ end if
│
└─ 通知服务器
   └─ server_.removeSession(session_id_)
```

#### N-S盒图

```
┌─────────────────────────────────────────────────────────────┐
│                    Session::stop()                           │
├─────────────────────────────────────────────────────────────┤
│ expected = false                                             │
├─────────────────────────────────────────────────────────────┤
│ if !stopped_.compare_exchange_strong(expected, true)        │
│   return (防止重复停止)                                      │
├─────────────────────────────────────────────────────────────┤
│ if connection_ != nullptr                                    │
│   connection_->stop()                                        │
├─────────────────────────────────────────────────────────────┤
│ server_.removeSession(session_id_)                           │
└─────────────────────────────────────────────────────────────┘
```

**方法签名:**
```cpp
void Session::stop() noexcept
```
- **前置条件:** 无
- **后置条件:** 连接已关闭,会话已从服务器移除
- **异常:** noexcept保证不抛出异常
- **线程安全:** 线程安全(使用原子操作)
- **幂等性:** 是(多次调用安全)

---

### 1.3 Session::onMessage() - 处理接收消息

#### PAD图

```
Session::onMessage(const string& message)
├─ 前置检查
│  ├─ if stopped_ || message.empty()
│  │  └─ return
│  └─ end if
│
├─ 执行命令
│  ├─ response = CommandRegistry::getInstance().executeCommand(this, message)
│  │
│  └─ if !response.empty()
│     └─ send(response)
│     └─ end if
│
└─ 结束
```

#### N-S盒图

```
┌─────────────────────────────────────────────────────────────┐
│              Session::onMessage(message)                     │
├─────────────────────────────────────────────────────────────┤
│ if stopped_ || message.empty()                               │
│   return                                                     │
├─────────────────────────────────────────────────────────────┤
│ response = CommandRegistry::getInstance()                    │
│                     .executeCommand(this, message)           │
├─────────────────────────────────────────────────────────────┤
│ if !response.empty()                                         │
│   send(response)                                             │
└─────────────────────────────────────────────────────────────┘
```

**方法签名:**
```cpp
void Session::onMessage(const std::string& message) noexcept
```
- **参数:** 
  - `message` [in] - 客户端发送的命令字符串
- **前置条件:** Session已启动
- **后置条件:** 命令已执行,响应已发送(如果有)
- **异常:** noexcept(命令执行的异常在CommandRegistry内部处理)
- **线程安全:** 线程安全(Connection的回调线程调用)

---

### 1.4 Session::setupEventListeners() - 注册事件监听

#### PAD图

```
Session::setupEventListeners()
├─ sessionId = session_id_
│
├─ 订阅CombatStartedEvent
│  └─ EventBus::subscribe(CombatStarted, 
│     └─ if event.sessionId == sessionId
│        └─ 构建战斗开始消息
│        └─ send(消息)
│        └─ end if
│
├─ 订阅CombatEndedEvent
│  └─ EventBus::subscribe(CombatEnded,
│     └─ if event.sessionId == sessionId
│        └─ if !event.playerWon
│           └─ setCurrentRoom("hall")
│           └─ end if
│        └─ end if
│
├─ 订阅PlayerDiedEvent
│  └─ EventBus::subscribe(PlayerDied,
│     └─ if event.sessionId == sessionId
│        └─ 构建死亡消息
│        └─ send(消息)
│        └─ player_.setCurrentHP(getMaxHP())
│        └─ player_.setCurrentMP(getMaxMP())
│        └─ end if
│
└─ 订阅MessageSentEvent
   └─ EventBus::subscribe(MessageSent,
      └─ if event.sessionId == sessionId
         └─ send(event.message)
         └─ end if
```

#### N-S盒图

```
┌─────────────────────────────────────────────────────────────┐
│              Session::setupEventListeners()                  │
├─────────────────────────────────────────────────────────────┤
│ sessionId = session_id_                                      │
├─────────────────────────────────────────────────────────────┤
│ subscribe(CombatStarted)  │ subscribe(CombatEnded)          │
├───────────────────────────┼─────────────────────────────────┤
│ if event.sessionId==id    │ if event.sessionId==id          │
│   构建战斗消息            │   if !event.playerWon           │
│   send()                  │     setCurrentRoom("hall")      │
├───────────────────────────┼─────────────────────────────────┤
│ subscribe(PlayerDied)     │ subscribe(MessageSent)          │
├───────────────────────────┼─────────────────────────────────┤
│ if event.sessionId==id    │ if event.sessionId==id          │
│   构建死亡消息            │   send(event.message)           │
│   send()                  │                                 │
│   restore HP/MP           │                                 │
└───────────────────────────┴─────────────────────────────────┘
```

**方法签名:**
```cpp
void Session::setupEventListeners() noexcept(false)
```
- **前置条件:** EventBus单例已初始化
- **后置条件:** 4个事件监听器已注册
- **异常:** 可能抛出std::bad_alloc(lambda创建失败)
- **线程安全:** 非线程安全,应在Session启动时单线程调用

---

## 2. CommandRegistry类 - 命令路由中心

### 2.1 CommandRegistry::executeCommand() - 执行命令

#### PAD图

```
CommandRegistry::executeCommand(Session* session, const string& input)
├─ 输入验证
│  ├─ if input.empty()
│  │  └─ return ""
│  └─ end if
│
├─ 解析输入
│  ├─ args = parseInput(input)  // 按空格分割
│  └─ if args.empty()
│     └─ return ""
│     └─ end if
│
├─ 命令查找
│  ├─ cmd = tolower(args[0])
│  ├─ it = commands_.find(cmd)
│  └─ if it != commands_.end()
│     ├─ return it->second->execute(session, args)
│     └─ else
│        └─ return "Unknown command..."
│     └─ end if
│
└─ 结束
```

#### N-S盒图

```
┌─────────────────────────────────────────────────────────────┐
│        CommandRegistry::executeCommand(session, input)       │
├─────────────────────────────────────────────────────────────┤
│ if input.empty()                                             │
│   return ""                                                  │
├─────────────────────────────────────────────────────────────┤
│ args = parseInput(input)                                     │
├─────────────────────────────────────────────────────────────┤
│ if args.empty()                                              │
│   return ""                                                  │
├─────────────────────────────────────────────────────────────┤
│ cmd = tolower(args[0])                                       │
├─────────────────────────────────────────────────────────────┤
│ if commands_.contains(cmd)                                   │
│   return commands_[cmd]->execute(session, args)              │
│ else                                                         │
│   return "Unknown command. Type 'help'..."                   │
└─────────────────────────────────────────────────────────────┘
```

**方法签名:**
```cpp
std::string CommandRegistry::executeCommand(
    Session* session, 
    const std::string& input
) noexcept(true)
```
- **参数:**
  - `session` [in] - 当前会话指针(非空)
  - `input` [in] - 用户输入的命令字符串
- **返回:** 命令执行的响应字符串
- **前置条件:** session != nullptr
- **后置条件:** 命令已执行,返回响应
- **异常:** noexcept(true)保证不抛出异常
- **线程安全:** 线程安全(只读查找,命令实现自行保证安全)

---

### 2.2 CommandRegistry::registerCommand() - 注册命令

#### PAD图

```
CommandRegistry::registerCommand(shared_ptr<ICommand> command)
├─ 提取命令名
│  ├─ name = command->getName()
│  └─ name = tolower(name)
│
├─ 注册到映射表
│  └─ commands_[name] = command
│
└─ 结束
```

#### N-S盒图

```
┌─────────────────────────────────────────────────────────────┐
│         CommandRegistry::registerCommand(command)            │
├─────────────────────────────────────────────────────────────┤
│ name = tolower(command->getName())                           │
├─────────────────────────────────────────────────────────────┤
│ commands_[name] = command                                    │
└─────────────────────────────────────────────────────────────┘
```

**方法签名:**
```cpp
void CommandRegistry::registerCommand(
    std::shared_ptr<ICommand> command
) noexcept(false)
```
- **参数:**
  - `command` [in] - 命令对象智能指针
- **前置条件:** command != nullptr
- **后置条件:** 命令已注册到commands_映射表
- **异常:** 可能抛出std::bad_alloc
- **线程安全:** 非线程安全(应在初始化阶段调用)

---

### 2.3 CommandRegistry::registerCommandWithAlias() - 注册带别名命令

#### PAD图

```
CommandRegistry::registerCommandWithAlias(command, aliases)
├─ 遍历别名列表
│  └─ for each alias in aliases
│     └─ commands_[alias] = command
│     └─ end for
│
└─ 结束
```

#### N-S盒图

```
┌─────────────────────────────────────────────────────────────┐
│    CommandRegistry::registerCommandWithAlias(command, aliases)│
├─────────────────────────────────────────────────────────────┤
│ for each alias in aliases                                    │
│   commands_[alias] = command                                 │
└─────────────────────────────────────────────────────────────┘
```

**方法签名:**
```cpp
void CommandRegistry::registerCommandWithAlias(
    std::shared_ptr<ICommand> command,
    const std::vector<std::string>& aliases
) noexcept(false)
```
- **参数:**
  - `command` [in] - 命令对象智能指针
  - `aliases` [in] - 别名列表(如{"north", "n"})
- **前置条件:** command != nullptr, !aliases.empty()
- **后置条件:** 命令及其所有别名已注册
- **异常:** 可能抛出std::bad_alloc

---

## 3. CombatManager类 - 战斗状态机

### 3.1 CombatManager::playerAttack() - 玩家攻击

#### PAD图

```
CombatManager::playerAttack(sessionId, playerAttack, playerDefense, playerHP)
├─ 初始化结果
│  └─ result = {sessionId, ...}
│
├─ 状态检查
│  ├─ if state_ != InCombat || !monster_.isAlive()
│  │  ├─ result.message = "You are not in combat."
│  │  └─ return result
│  └─ end if
│
├─ 计算伤害(策略模式)
│  ├─ strategy = PhysicalDamageStrategy
│  ├─ damage = strategy.calculateDamage(playerAttack, monster_.getDefense(), isCrit)
│  └─ monster_.takeDamage(damage)
│
├─ 构建攻击消息
│  └─ result.message = "You attack... {damage} damage!"
│
├─ 发布攻击事件
│  └─ EventBus::publish(PlayerAttackedEvent)
│
├─ 检查怪物生死
│  ├─ if monster_.isDead()
│  │  ├─ result.playerWon = true
│  │  ├─ result.expGained = monster_.getExpReward()
│  │  ├─ result.message = "*** VICTORY! ***..."
│  │  └─ endCombat(sessionId, true)
│  │
│  └─ else
│     ├─ 怪物反击
│     │  └─ monsterResult = monsterAttack(sessionId, playerDefense, playerHP)
│     │  └─ result.damageReceived = monsterResult.damageDealt
│     │  └─ result.message += monsterResult.message
│     │
│     └─ 检查玩家生死
│        └─ if playerHP - result.damageReceived <= 0
│           ├─ result.playerWon = false
│           └─ endCombat(sessionId, false)
│        └─ end if
│     └─ end else
│  └─ end if
│
└─ return result
```

#### N-S盒图

```
┌─────────────────────────────────────────────────────────────┐
│ CombatManager::playerAttack(sessionId, atk, def, hp)         │
├─────────────────────────────────────────────────────────────┤
│ result = {sessionId, ...}                                    │
├─────────────────────────────────────────────────────────────┤
│ if state_ != InCombat || !monster_.isAlive()                 │
│   result.message = "You are not in combat."                  │
│   return result                                              │
├─────────────────────────────────────────────────────────────┤
│ damage = PhysicalDamageStrategy.calculateDamage(atk, def)   │
├─────────────────────────────────────────────────────────────┤
│ monster_.takeDamage(damage)                                  │
├─────────────────────────────────────────────────────────────┤
│ publish(PlayerAttackedEvent)                                 │
├─────────────────────────────────────────────────────────────┤
│ if monster_.isDead()                                         │
│   result.playerWon = true                                    │
│   result.expGained = monster_.getExpReward()                 │
│   endCombat(sessionId, true)                                 │
│ else                                                         │
│   monsterResult = monsterAttack(sessionId, def, hp)          │
│   result.damageReceived = monsterResult.damageDealt          │
│   result.message += monsterResult.message                    │
│   if hp - damageReceived <= 0                                │
│     result.playerWon = false                                 │
│     endCombat(sessionId, false)                              │
├─────────────────────────────────────────────────────────────┤
│ return result                                                │
└─────────────────────────────────────────────────────────────┘
```

**方法签名:**
```cpp
CombatResult CombatManager::playerAttack(
    uint32_t sessionId,
    int playerAttack,
    int playerDefense,
    int playerHP
) noexcept(true)
```
- **参数:**
  - `sessionId` [in] - 会话ID
  - `playerAttack` [in] - 玩家攻击力
  - `playerDefense` [in] - 玩家防御力
  - `playerHP` [in] - 玩家当前HP
- **返回:** CombatResult战斗结果结构
- **前置条件:** state_ == InCombat, monster_.isAlive()
- **后置条件:** 伤害已计算,事件已发布,可能结束战斗
- **异常:** noexcept(true)
- **线程安全:** 非线程安全(每个Session独立CombatManager实例)

---

### 3.2 CombatManager::monsterAttack() - 怪物攻击

#### PAD图

```
CombatManager::monsterAttack(sessionId, playerDefense, playerHP)
├─ 初始化结果
│  └─ result = {sessionId, ...}
│
├─ 状态检查
│  └─ if state_ != InCombat || !monster_.isAlive()
│     └─ return result
│  └─ end if
│
├─ 计算怪物伤害
│  ├─ damage = monster_.attack()
│  └─ actual_damage = max(1, damage - playerDefense)
│
├─ 构建攻击消息
│  ├─ message = "The {monster} attacks... {damage} damage!"
│  └─ 根据remaining_hp添加状态描述
│     └─ if hp > 80: "barely feel"
│     └─ else if hp > 60: "minor hit"
│     └─ else if hp > 40: "feel pain"
│     └─ else if hp > 20: "badly hurt"
│     └─ else: "brink of death"
│
├─ 发布攻击事件
│  └─ EventBus::publish(MonsterAttackedEvent)
│
└─ return result
```

#### N-S盒图

```
┌─────────────────────────────────────────────────────────────┐
│ CombatManager::monsterAttack(sessionId, def, hp)             │
├─────────────────────────────────────────────────────────────┤
│ if state_ != InCombat || !monster_.isAlive()                 │
│   return result                                              │
├─────────────────────────────────────────────────────────────┤
│ damage = monster_.attack()                                   │
│ actual_damage = max(1, damage - def)                         │
├─────────────────────────────────────────────────────────────┤
│ remaining_hp = hp - actual_damage                            │
├──────────────────────┬──────────────────────────────────────┤
│ remaining_hp > 80    │ "barely feel the wound"              │
│ 60 < hp <= 80        │ "take a minor hit"                   │
│ 40 < hp <= 60        │ "starting to feel the pain"          │
│ 20 < hp <= 40        │ "badly hurt"                         │
│ hp <= 20             │ "brink of death"                     │
├──────────────────────┴──────────────────────────────────────┤
│ publish(MonsterAttackedEvent)                                │
├─────────────────────────────────────────────────────────────┤
│ return result                                                │
└─────────────────────────────────────────────────────────────┘
```

**方法签名:**
```cpp
CombatResult CombatManager::monsterAttack(
    uint32_t sessionId,
    int playerDefense,
    int playerHP
) noexcept(true)
```
- **参数:**
  - `sessionId` [in] - 会话ID
  - `playerDefense` [in] - 玩家防御力
  - `playerHP` [in] - 玩家当前HP
- **返回:** CombatResult战斗结果
- **前置条件:** state_ == InCombat
- **后置条件:** 伤害已计算,事件已发布
- **异常:** noexcept(true)

---

### 3.3 CombatManager::startCombat() - 开始战斗

#### PAD图

```
CombatManager::startCombat(sessionId, monster)
├─ 初始化战斗状态
│  ├─ monster_ = monster
│  └─ state_ = InCombat
│
├─ 发布战斗开始事件
│  └─ event = CombatStartedEvent(sessionId, monster_.getName(), monster_.getDesc())
│  └─ EventBus::publish(event)
│
└─ 结束
```

#### N-S盒图

```
┌─────────────────────────────────────────────────────────────┐
│ CombatManager::startCombat(sessionId, monster)               │
├─────────────────────────────────────────────────────────────┤
│ monster_ = monster                                           │
├─────────────────────────────────────────────────────────────┤
│ state_ = CombatState::InCombat                               │
├─────────────────────────────────────────────────────────────┤
│ event = CombatStartedEvent(sessionId, name, desc)            │
│ EventBus::getInstance().publish(event)                       │
└─────────────────────────────────────────────────────────────┘
```

**方法签名:**
```cpp
void CombatManager::startCombat(
    uint32_t sessionId,
    Monster monster
) noexcept(false)
```
- **参数:**
  - `sessionId` [in] - 会话ID
  - `monster` [in] - 怪物对象(值传递)
- **前置条件:** state_ == NotInCombat
- **后置条件:** state_ == InCombat, CombatStartedEvent已发布
- **异常:** 可能抛出EventBus相关异常

---

### 3.4 CombatManager::endCombat() - 结束战斗

#### PAD图

```
CombatManager::endCombat(sessionId, playerWon)
├─ 判断胜负
│  ├─ if playerWon
│  │  └─ state_ = CombatWon
│  │
│  └─ else
│     ├─ state_ = CombatLost
│     ├─ 发布PlayerDiedEvent
│     └─ end if
│
├─ 计算经验奖励
│  └─ exp = playerWon ? monster_.getExpReward() : 0
│
├─ 发布战斗结束事件
│  └─ event = CombatEndedEvent(sessionId, playerWon, exp, monster_.getName())
│  └─ EventBus::publish(event)
│
├─ 重置状态
│  └─ state_ = NotInCombat
│
└─ 结束
```

#### N-S盒图

```
┌─────────────────────────────────────────────────────────────┐
│ CombatManager::endCombat(sessionId, playerWon)               │
├──────────────────────┬──────────────────────────────────────┤
│ playerWon == true    │ state_ = CombatWon                   │
│ playerWon == false   │ state_ = CombatLost                  │
│                      │ publish(PlayerDiedEvent)             │
├──────────────────────┴──────────────────────────────────────┤
│ exp = playerWon ? monster_.getExpReward() : 0                │
├─────────────────────────────────────────────────────────────┤
│ publish(CombatEndedEvent(sessionId, playerWon, exp, name))   │
├─────────────────────────────────────────────────────────────┤
│ state_ = CombatState::NotInCombat                            │
└─────────────────────────────────────────────────────────────┘
```

**方法签名:**
```cpp
void CombatManager::endCombat(
    uint32_t sessionId,
    bool playerWon
) noexcept(false)
```
- **参数:**
  - `sessionId` [in] - 会话ID
  - `playerWon` [in] - 玩家是否胜利
- **前置条件:** state_ == InCombat
- **后置条件:** state_ == NotInCombat, CombatEndedEvent已发布
- **异常:** 可能抛出EventBus相关异常

---

## 4. EventBus类 - 事件发布订阅

### 4.1 EventBus::subscribe() - 订阅事件

#### PAD图

```
EventBus::subscribe(EventType type, EventHandler handler)
├─ 加锁
│  └─ lock_guard<mutex> lock(mutex_)
│
├─ 添加到处理器列表
│  └─ handlers_[type].push_back(handler)
│
└─ 结束
```

#### N-S盒图

```
┌─────────────────────────────────────────────────────────────┐
│ EventBus::subscribe(type, handler)                           │
├─────────────────────────────────────────────────────────────┤
│ lock_guard<mutex> lock(mutex_)                               │
├─────────────────────────────────────────────────────────────┤
│ handlers_[type].push_back(move(handler))                     │
└─────────────────────────────────────────────────────────────┘
```

**方法签名:**
```cpp
void EventBus::subscribe(
    EventType type,
    EventHandler handler
) noexcept(false)
```
- **参数:**
  - `type` [in] - 事件类型枚举
  - `handler` [in] - 事件处理函数对象
- **前置条件:** 无
- **后置条件:** 处理器已添加到对应事件类型的列表
- **异常:** 可能抛出std::bad_alloc
- **线程安全:** 线程安全(使用mutex保护)

---

### 4.2 EventBus::publish() - 发布事件

#### PAD图

```
EventBus::publish(const Event& event)
├─ 加锁
│  └─ lock_guard<mutex> lock(mutex_)
│
├─ 查找处理器
│  ├─ it = handlers_.find(event.getType())
│  └─ if it != handlers_.end()
│     └─ for each handler in it->second
│        ├─ try
│        │  └─ handler(event)
│        │
│        ├─ catch (const exception& e)
│        │  └─ cerr << "Event handler error: " << e.what()
│        │
│        └─ end try-catch
│     └─ end for
│  └─ end if
│
└─ 结束
```

#### N-S盒图

```
┌─────────────────────────────────────────────────────────────┐
│ EventBus::publish(event)                                     │
├─────────────────────────────────────────────────────────────┤
│ lock_guard<mutex> lock(mutex_)                               │
├─────────────────────────────────────────────────────────────┤
│ if handlers_.contains(event.getType())                       │
│   for each handler in handlers_[event.getType()]             │
│     try                                                      │
│       handler(event)                                         │
│     catch (const exception& e)                               │
│       cerr << "Event handler error: " << e.what()            │
└─────────────────────────────────────────────────────────────┘
```

**方法签名:**
```cpp
void EventBus::publish(
    const Event& event
) noexcept(true)
```
- **参数:**
  - `event` [in] - 事件对象引用(基类Event)
- **前置条件:** 无
- **后置条件:** 所有订阅者已处理事件(或捕获异常)
- **异常:** noexcept(true)保证不抛出异常(内部try-catch)
- **线程安全:** 线程安全

---

## 5. Connection类 - 网络I/O

### 5.1 Connection::readLoop() - 读取循环

#### PAD图

```
Connection::readLoop()
├─ 初始化缓冲区
│  └─ buffer[1024], line_buffer
│
├─ while !stopped_
│  ├─ bytes_read = network::recv_data(socket_fd_, buffer, 1024)
│  │
│  ├─ if bytes_read <= 0
│  │  └─ break
│  │  └─ end if
│  │
│  └─ for i = 0 to bytes_read
│     ├─ c = buffer[i]
│     │
│     ├─ if c == '\n' || c == '\r'
│     │  ├─ if !line_buffer.empty()
│     │  │  ├─ if on_message_
│     │  │  │  └─ on_message_(line_buffer)
│     │  │  │  └─ end if
│     │  │  └─ line_buffer.clear()
│     │  │  └─ end if
│     │  │
│     │  └─ else
│     │     └─ line_buffer += c
│     │  └─ end if
│     │
│     └─ end for
│  │
│  └─ end while
│
└─ stop()
```

#### N-S盒图

```
┌─────────────────────────────────────────────────────────────┐
│ Connection::readLoop()                                       │
├─────────────────────────────────────────────────────────────┤
│ while !stopped_                                              │
│   bytes_read = recv_data(socket_fd_, buffer, 1024)           │
├─────────────────────────────────────────────────────────────┤
│   if bytes_read <= 0                                         │
│     break                                                    │
├─────────────────────────────────────────────────────────────┤
│   for i = 0 to bytes_read                                    │
│     c = buffer[i]                                            │
├──────────────────────────┬──────────────────────────────────┤
│     if c == '\n' or '\r' │ else                              │
│       if !line_empty()   │ line_buffer += c                  │
│         if on_message_   │                                   │
│           on_message_()  │                                   │
│         line_buffer.clear│                                   │
├──────────────────────────┴──────────────────────────────────┤
│ stop()                                                       │
└─────────────────────────────────────────────────────────────┘
```

**方法签名:**
```cpp
void Connection::readLoop() noexcept
```
- **前置条件:** socket_fd_有效,网络已初始化
- **后置条件:** 连接已关闭(循环退出时调用stop())
- **异常:** noexcept(所有异常在内部处理)
- **线程安全:** 在独立线程中运行

---

## 6. 跨类调用方法签名规范

### 6.1 Session → CommandRegistry

```cpp
// 调用方式
std::string response = CommandRegistry::getInstance()
    .executeCommand(this, message);

// 签名规范
std::string CommandRegistry::executeCommand(
    Session* session,              // [in] 会话指针(非空)
    const std::string& input       // [in] 用户输入
) noexcept(true);

// 契约
// 前置: session != nullptr
// 后置: 返回命令执行结果字符串
// 异常: 无(noexcept)
// 线程: 线程安全
```

---

### 6.2 Session → EventBus

```cpp
// 调用方式
EventBus::getInstance().subscribe(EventType::CombatStarted, 
    [this, sessionId](const Event& e) {
        const auto& event = static_cast<const CombatStartedEvent&>(e);
        // 处理逻辑
    });

// 签名规范
void EventBus::subscribe(
    EventType type,                // [in] 事件类型
    EventHandler handler           // [in] 处理函数
) noexcept(false);

void EventBus::publish(
    const Event& event             // [in] 事件对象
) noexcept(true);

// 契约
// subscribe前置: 无
// subscribe后置: handler已添加到type对应的列表
// publish前置: 无
// publish后置: 所有订阅者已处理
// subscribe线程: 线程安全
// publish线程: 线程安全
```

---

### 6.3 Command → Session

```cpp
// 调用示例(LookCommand)
const Room* room = session->getCurrentRoom();
auto players = session->getServer().getPlayersInRoom(room->getId(), session->getId());

// 签名规范
const Room* Session::getCurrentRoom() const noexcept(true);
Room* Session::getCurrentRoom() noexcept(true);

void Session::setCurrentRoom(
    const std::string& room_id     // [in] 房间ID
) noexcept(true);

MudServer& Session::getServer() noexcept(true);
const MudServer& Session::getServer() const noexcept(true);

// 契约
// getCurrentRoom前置: 当前房间已设置
// getCurrentRoom后置: 返回房间指针(可能nullptr)
// setCurrentRoom前置: room_id有效
// setCurrentRoom后置: 当前房间已更新,player.location已更新
// 异常: 全部noexcept
```

---

### 6.4 Command → CombatManager

```cpp
// 调用示例(KillCommand)
session->getCombatManager().startCombat(session->getId(), monster);

// 签名规范
CombatManager& Session::getCombatManager() noexcept(true);
bool Session::isInCombat() const noexcept(true);

void CombatManager::startCombat(
    uint32_t sessionId,            // [in] 会话ID
    Monster monster                // [in] 怪物对象
) noexcept(false);

CombatResult CombatManager::playerAttack(
    uint32_t sessionId,            // [in] 会话ID
    int playerAttack,              // [in] 玩家攻击力
    int playerDefense,             // [in] 玩家防御力
    int playerHP                   // [in] 玩家当前HP
) noexcept(true);

// 契约
// startCombat前置: state_ == NotInCombat
// startCombat后置: state_ == InCombat,事件已发布
// playerAttack前置: state_ == InCombat
// playerAttack后置: 伤害已计算,可能结束战斗
```

---

### 6.5 CombatManager → EventBus

```cpp
// 调用方式
CombatStartedEvent event(sessionId, monsterName, monsterDesc);
EventBus::getInstance().publish(event);

// 事件定义规范
class CombatStartedEvent : public Event {
public:
    uint32_t sessionId;            // 会话ID
    std::string monsterName;       // 怪物名称
    std::string monsterDescription;// 怪物描述
    
    CombatStartedEvent(
        uint32_t sid,              // [in] 会话ID
        const std::string& mname,  // [in] 怪物名
        const std::string& mdesc   // [in] 怪物描述
    );
    
    EventType getType() const override;
};

// 契约
// 构造函数前置: 无
// 构造函数后置: 所有字段已初始化
// 事件发布: 通过EventBus::publish()异步分发
```

---

### 6.6 Session → Connection

```cpp
// 调用方式
connection_->onMessage([this](const std::string& msg) {
    onMessage(msg);
});
connection_->start();

// 签名规范
void Connection::onMessage(
    MessageCallback callback       // [in] 消息回调函数
) noexcept(true);

void Connection::onClose(
    CloseCallback callback         // [in] 关闭回调函数
) noexcept(true);

void Connection::start() noexcept(false);
void Connection::stop() noexcept;
void Connection::send(
    const std::string& message     // [in] 消息内容
) noexcept(true);

// 契约
// onMessage前置: 无
// onMessage后置: callback已设置
// start前置: socket_fd_有效
// start后置: 读写线程已启动
// stop前置: 无
// stop后置: 连接已关闭,回调已触发
// send前置: 无
// send后置: 消息已加入队列
```

---

## 7. 数据流图 (Data Flow Diagram)

### 7.1 客户端消息处理流程

```
┌──────────┐     TCP      ┌──────────┐    Callback   ┌──────────┐
│  Client  │ ──────────→ │Connection│ ─────────────→ │ Session  │
│          │ ←────────── │          │ ←───────────── │          │
└──────────┘   Response  └──────────┘   send()      └────┬─────┘
                                                          │
                                               Execute    │
                                                          ▼
                                                    ┌──────────┐
                                                    │ Command  │
                                                    │ Registry │
                                                    └────┬─────┘
                                                         │
                                                  Execute │
                                                          ▼
                                                    ┌──────────┐
                                                    │ ICommand │
                                                    │  (具体)  │
                                                    └──────────┘
```

### 7.2 战斗事件流

```
┌──────────┐  startCombat  ┌──────────────┐  publish  ┌──────────┐
│ Command  │ ───────────→ │CombatManager │ ─────────→ │ EventBus │
│          │               │              │           │          │
└──────────┘               └──────┬───────┘           └────┬─────┘
                                  │                        │
                           attack │                   subscribe
                                  │                        │
                                  ▼                        ▼
                            ┌──────────┐            ┌──────────┐
                            │ Strategy │            │ Session  │
                            │ (伤害)   │            │ (监听器) │
                            └──────────┘            └──────────┘
```

---

## 8. 类交互序列图 (Sequence Diagram)

### 8.1 命令执行序列

```
Client      Connection      Session     CommandRegistry   ICommand
  │             │             │              │               │
  │──"look"──→│             │              │               │
  │             │──onMessage─→│              │               │
  │             │             │──executeCmd─→│               │
  │             │             │              │──execute()──→│
  │             │             │              │               │──执行逻辑
  │             │             │←──response──│←──response──│
  │             │             │──send()──→│               │
  │←──response─│             │              │               │
```

### 8.2 战斗序列

```
Client    Session   CombatManager   Strategy   EventBus
  │         │          │              │           │
  │─"kill G"→         │              │           │
  │         │─startCombat→           │           │
  │         │          │──publish───→│           │
  │         │←──event─│              │           │
  │←─显示──│          │              │           │
  │         │          │              │           │
  │─"attack"→         │              │           │
  │         │─playerAttack→          │           │
  │         │          │─calcDmg──→│           │
  │         │          │←─damage───│           │
  │         │          │──publish───→│           │
  │         │←──result │              │           │
  │←─战斗结果│          │              │           │
```

---

## 9. 复杂度度量汇总

### 9.1 方法V(G)统计

| 类 | 方法 | V(G) | 行数 | 参数数 | 异常规范 |
|----|------|------|------|--------|----------|
| Session | start() | 3 | 30 | 0 | noexcept(false) |
| Session | stop() | 3 | 12 | 0 | noexcept |
| Session | onMessage() | 4 | 10 | 1 | noexcept |
| Session | setupEventListeners() | 12 | 50 | 0 | noexcept(false) |
| CommandRegistry | executeCommand() | 6 | 25 | 2 | noexcept(true) |
| CommandRegistry | registerCommand() | 2 | 5 | 1 | noexcept(false) |
| CombatManager | playerAttack() | 9 | 50 | 4 | noexcept(true) |
| CombatManager | monsterAttack() | 9 | 35 | 3 | noexcept(true) |
| CombatManager | startCombat() | 1 | 8 | 2 | noexcept(false) |
| CombatManager | endCombat() | 4 | 18 | 2 | noexcept(false) |
| EventBus | subscribe() | 1 | 4 | 2 | noexcept(false) |
| EventBus | publish() | 4 | 12 | 1 | noexcept(true) |

### 9.2 跨类调用依赖矩阵

```
调用方 \ 被调用方  │ Session │ CommandReg │ CombatMgr │ EventBus │ Connection
───────────────────┼─────────┼────────────┼───────────┼──────────┼──────────
Session            │   -     │     ✓      │     ✓     │    ✓     │    ✓
Command            │   ✓     │     -      │     -     │    -     │    -
CombatManager      │   -     │     -      │     -     │    ✓     │    -
EventBus           │   -     │     -      │     -     │    -     │    -
Connection         │   ✓     │     -      │     -     │    -     │    -
```

**依赖方向:**
```
Session → CommandRegistry (命令执行)
Session → CombatManager (战斗管理)
Session → EventBus (事件订阅)
Session → Connection (网络I/O)
Command → Session (上下文获取)
CombatManager → EventBus (事件发布)
Connection → Session (回调)
```

**循环依赖:** 0个 ✅

---

## 10. 接口契约总结

### 10.1 核心接口: ICommand

```cpp
class ICommand {
public:
    virtual ~ICommand() = default;
    
    /**
     * @brief 执行命令
     * @param session [in] 当前会话指针(非空)
     * @param args [in] 命令参数列表(第0个元素是命令名)
     * @return 执行结果字符串
     * @pre session != nullptr
     * @post 返回命令执行结果(可能为空字符串)
     * @throws 无保证(noexcept视具体实现而定)
     */
    virtual std::string execute(
        Session* session, 
        const std::vector<std::string>& args
    ) = 0;
    
    /**
     * @brief 获取命令帮助
     * @return 帮助字符串
     */
    virtual std::string getHelp() const = 0;
    
    /**
     * @brief 获取命令名称
     * @return 命令名(小写)
     */
    virtual std::string getName() const = 0;
};
```

### 10.2 事件基类: Event

```cpp
class Event {
public:
    virtual ~Event() = default;
    
    /**
     * @brief 获取事件类型
     * @return 事件类型枚举
     */
    virtual EventType getType() const = 0;
};
```

### 10.3 伤害策略接口: DamageStrategy

```cpp
class DamageStrategy {
public:
    virtual ~DamageStrategy() = default;
    
    /**
     * @brief 计算伤害值
     * @param attack [in] 攻击力
     * @param defense [in] 防御力
     * @param isCrit [out] 是否暴击(引用返回)
     * @return 实际伤害值
     * @pre attack > 0
     * @post isCrit已设置,返回值 >= 1
     */
    virtual int calculateDamage(
        int attack, 
        int defense, 
        bool& isCrit
    ) = 0;
    
    /**
     * @brief 获取策略名称
     * @return 策略名(如"Physical", "Magic")
     */
    virtual std::string getStrategyName() const = 0;
};
```

---

## ✅ 文档版本

- **版本:** 1.0
- **日期:** 2026年4月8日
- **状态:** ✅ 已完成
- **维护:** 每次修改核心类时需同步更新本文档
