# MUD 游戏核心用例文档

**版本:** 1.0  
**日期:** 2026-04-01  
**模块:** 用户服务 | 地图 | 战斗 | 保存

---

## 图例说明

| 标识 | 含义 |
|------|------|
| ✅ **Include** | 主干功能 - MVP 必须实现的核心用例 |
| 🔶 **Extend** | 扩展功能 - 可选的增强功能 |

---

## 1. 用户服务模块 (User Service)

### 功能概述
管理玩家会话、角色属性、社交互动等核心用户功能。

### 核心类
- `Player` - 玩家数据与行为
- `Session` - 玩家会话管理
- `MudServer` - 服务器与多会话管理
- `CommandHandler` - 命令解析与分发

---

### UC-USER-01: 玩家登录游戏

| 属性 | 描述 |
|------|------|
| **类型** | ✅ Include (主干) |
| **参与者** | 玩家、MudServer、Session |
| **前置条件** | 服务器已启动并监听端口 |
| **后置条件** | 玩家建立会话，可接收命令输入 |

**主流程:**
```
1. 玩家通过 TCP 客户端连接到服务器 (默认端口 8888)
2. 服务器接受连接，创建新的 Session 对象
3. 服务器分配唯一 session_id
4. Session 发送欢迎消息给玩家
5. 玩家进入未命名状态 (默认名 "Stranger")
```

**欢迎消息示例:**
```
+-------------------------------------------------------+
|     Welcome to Western Fantasy MUD Game!              |
+-------------------------------------------------------+

You are an adventurer entering this mysterious world.
Explore unknown lands, meet other adventurers,
and experience a wonderful fantasy journey.

Type 'help' to see available commands.
Type 'name <yourname>' to set your character name.
```

**扩展流程:**
- 🔶 登录时自动加载上次存档
- 🔶 登录失败重试机制
- 🔶 多角色选择界面

---

### UC-USER-02: 设置角色名称

| 属性 | 描述 |
|------|------|
| **类型** | ✅ Include (主干) |
| **参与者** | 玩家、Session、Player |
| **前置条件** | 玩家已连接但未设置名称 |
| **后置条件** | 玩家获得有效名称，标记为 logged_in=true |

**主流程:**
```
1. 玩家输入命令：name <角色名>
2. 系统验证名称长度 (2-20 字符)
3. 更新 Player.name_
4. 设置 Player.logged_in_ = true
5. 向同房间玩家广播加入消息
6. 返回确认消息
```

**命令示例:**
```
> name Arthur
Your name has been changed from "Stranger" to "Arthur".
Arthur has entered the game.
```

**异常流程:**
- 名称过短/过长 → 提示 "Name must be between 2-20 characters."
- 名称包含非法字符 → 自动转换为下划线

**扩展流程:**
- 🔶 名称唯一性检查
- 🔶 敏感词过滤
- 🔶 保留名称保护

---

### UC-USER-03: 查看在线玩家

| 属性 | 描述 |
|------|------|
| **类型** | ✅ Include (主干) |
| **参与者** | 玩家、MudServer |
| **前置条件** | 玩家已登录 |
| **后置条件** | 显示当前在线玩家列表 |

**主流程:**
```
1. 玩家输入命令：who
2. 服务器遍历所有活跃 Session
3. 收集所有 Player.name
4. 返回格式化列表
```

**命令示例:**
```
> who
====== Online Players ======
  - Arthur
  - Lancelot
  - Merlin
==========================
```

**扩展流程:**
- 🔶 显示玩家等级/位置
- 🔶 好友在线提醒
- 🔶 玩家隐身模式

---

### UC-USER-04: 查看玩家状态

| 属性 | 描述 |
|------|------|
| **类型** | ✅ Include (主干) |
| **参与者** | 玩家、Player |
| **前置条件** | 玩家已登录 |
| **后置条件** | 显示玩家完整属性 |

**主流程:**
```
1. 玩家输入命令：score
2. 系统读取 Player 所有属性
3. 返回格式化状态面板
```

**命令示例:**
```
> score
====== Arthur's Status ======
  Level: 5
  Experience: 250
  Location: Crystal Corridor
  HP: 180/180
  MP: 90/90
  Attack: 18
  Defense: 9
  Gold: 100
  Items: 3
============================================
```

**扩展流程:**
- 🔶 显示详细抗性/装备
- 🔶 显示战斗统计 (击杀数/死亡数)
- 🔶 成就系统

---

### UC-USER-05: 房间聊天

| 属性 | 描述 |
|------|------|
| **类型** | ✅ Include (主干) |
| **参与者** | 玩家、Session、MudServer |
| **前置条件** | 玩家位于某个房间 |
| **后置条件** | 同房间玩家收到消息 |

**主流程:**
```
1. 玩家输入命令：say <消息内容>
2. 系统获取玩家当前房间
3. 向房间内所有其他玩家广播消息
4. 向发送者返回确认
```

**命令示例:**
```
> say Hello, fellow adventurers!
You say: "Hello, fellow adventurers!"
Merlin says: "Greetings, Arthur!"
```

**扩展流程:**
- 🔶 全服广播频道 (`shout`)
- 🔶 私聊功能 (`tell <玩家> <消息>`)
- 🔶 表情动作 (`emote 向众人鞠躬`)

---

### UC-USER-06: 查看背包物品

| 属性 | 描述 |
|------|------|
| **类型** | ✅ Include (主干) |
| **参与者** | 玩家、Player |
| **前置条件** | 玩家已登录 |
| **后置条件** | 显示背包物品列表 |

**主流程:**
```
1. 玩家输入命令：inventory 或 i
2. 系统读取 Player.inventory_
3. 返回格式化物品列表
```

**命令示例:**
```
> i
====== Inventory ======
You are carrying:
  - Iron Sword: A sturdy blade forged from iron
  - Health Potion: Restores 50 HP when consumed
  - Gold Ring: A simple ring with minor enchantment
=======================
```

**扩展流程:**
- 🔶 物品详细属性查看
- 🔶 物品排序/分类
- 🔶 装备槽位系统

---

## 2. 地图模块 (Map/World)

### 功能概述
管理游戏世界、房间结构、NPC 与怪物分布。

### 核心类
- `Room` - 房间数据结构
- `World` - 世界地图管理
- `NPC` - NPC 数据结构
- `Monster` - 怪物数据 (与战斗模块共享)

---

### UC-MAP-01: 查看房间信息

| 属性 | 描述 |
|------|------|
| **类型** | ✅ Include (主干) |
| **参与者** | 玩家、Session、Room |
| **前置条件** | 玩家位于某个房间 |
| **后置条件** | 显示房间完整信息 |

**主流程:**
```
1. 玩家输入命令：look
2. 系统获取玩家当前 Room 对象
3. 返回房间名称、描述、出口、NPC、怪物、其他玩家
```

**命令示例:**
```
> look
====== Grand Starting Hall ======

You stand in the center of a grand stone hall. Ancient banners hang on the walls,
depicting brave knights fighting dragons. The hall is bustling with adventurers.
A corridor leads north, a small room is to the east, and the sound of flowing
water comes from the west.

Visible exits: north, east, west

You see here:
  - Elder Sage: An old sage with a long white beard

Other adventurers here:
  - Merlin
```

**扩展流程:**
- 🔶 可调查物品 (`examine <物品>`)
- 🔶 隐藏出口发现
- 🔶 房间互动元素

---

### UC-MAP-02: 房间移动

| 属性 | 描述 |
|------|------|
| **类型** | ✅ Include (主干) |
| **参与者** | 玩家、Session、Room、World |
| **前置条件** | 玩家位于有出口的房间 |
| **后置条件** | 玩家位置更新，原/新房间玩家收到通知 |

**主流程:**
```
1. 玩家输入移动命令：north/n, south/s, east/e, west/w
2. 系统检查当前房间是否有该方向出口
3. 若有出口，更新玩家位置到新房间
4. 向原房间广播离开消息
5. 向新房间广播到达消息
6. 返回新房间信息
```

**命令示例:**
```
> north
Arthur leaves north.

You walk north and arrive at Crystal Corridor.
Arthur arrives from south.

====== Crystal Corridor ======

A long corridor with magical crystals glowing softly on the walls.
The air is filled with mysterious energy...

Visible exits: south, north

*** DANGEROUS CREATURES ***
  - Goblin: A small green creature with beady eyes and a rusty dagger
```

**异常流程:**
- 该方向无出口 → 提示 "You cannot go that way."

**扩展流程:**
- 🔶 上/下移动 (`up`, `down`)
- 🔶 进入/离开建筑 (`enter`, `exit`)
- 🔶 移动消耗 (体力/时间)
- 🔶 随机遭遇事件

---

### UC-MAP-03: 与 NPC 交互

| 属性 | 描述 |
|------|------|
| **类型** | 🔶 Extend (扩展) |
| **参与者** | 玩家、Room、NPC |
| **前置条件** | 玩家房间内有目标 NPC |
| **后置条件** | 触发 NPC 对话或功能 |

**主流程:**
```
1. 玩家输入命令：talk <NPC 名>
2. 系统查找房间内匹配 NPC
3. 返回 NPC 问候语或交互选项
```

**命令示例:**
```
> talk Elder Sage
Elder Sage says: "Welcome, young adventurer! The world awaits you.
Many dangers lurk beyond these halls. Be sure to explore the corridor
to the north, but beware of the goblins that have taken residence there."
```

**扩展流程:**
- 🔶 多轮对话树
- 🔶 NPC 交易功能
- 🔶 任务系统
- 🔶 NPC 好感度

---

### UC-MAP-04: 发现隐藏房间

| 属性 | 描述 |
|------|------|
| **类型** | 🔶 Extend (扩展) |
| **参与者** | 玩家、Room、World |
| **前置条件** | 玩家位于有隐藏出口的房间 |
| **后置条件** | 新房间/出口被解锁 |

**主流程:**
```
1. 玩家输入命令：search 或 examine <线索>
2. 系统检查房间是否有隐藏出口
3. 若发现，解锁新出口或房间
4. 返回发现消息
```

**命令示例:**
```
> examine ancient symbols
You notice the symbols form a pattern pointing to the western wall.
When you touch the wall, a hidden door slides open!

A secret passage leads downward...
```

**扩展流程:**
- 🔶 需要特定物品开启
- 🔶 需要解谜
- 🔶 限时隐藏房间

---

## 3. 战斗模块 (Combat)

### 功能概述
管理玩家与怪物的回合制战斗，包括攻击、防御、逃跑、经验获取。

### 核心类
- `CombatManager` - 战斗状态管理
- `Monster` - 怪物数据与行为
- `CombatResult` - 战斗结果结构

---

### UC-COMBAT-01: 发起战斗

| 属性 | 描述 |
|------|------|
| **类型** | ✅ Include (主干) |
| **参与者** | 玩家、Session、CombatManager、Room、Monster |
| **前置条件** | 玩家房间内有存活的怪物 |
| **后置条件** | 进入战斗状态，怪物从房间移除 |

**主流程:**
```
1. 玩家输入命令：kill <怪物名>
2. 系统检查玩家是否已在战斗中
3. 系统在房间内查找目标怪物
4. 创建 CombatManager 实例，设置战斗状态为 InCombat
5. 从房间移除怪物 (逻辑上)
6. 返回战斗开始消息
```

**命令示例:**
```
> kill Goblin
====== COMBAT STARTED ======

A Goblin appears before you!
A small green creature with beady eyes and a rusty dagger

Goblin looks healthy and ready to fight!

Type 'kill Goblin' or 'attack' to fight!
============================
```

**异常流程:**
- 已在战斗中 → 提示 "You are already in combat!"
- 怪物不存在 → 提示 "There is no such monster here."

**扩展流程:**
- 🔶 群体战斗 (多个怪物)
- 🔶 玩家组队战斗
- 🔶 先攻判定 (速度属性)

---

### UC-COMBAT-02: 玩家攻击

| 属性 | 描述 |
|------|------|
| **类型** | ✅ Include (主干) |
| **参与者** | 玩家、CombatManager、Monster |
| **前置条件** | 玩家处于战斗状态 |
| **后置条件** | 怪物 HP 减少，可能触发怪物反击或战斗结束 |

**主流程:**
```
1. 玩家输入命令：attack 或 kill <怪物名>
2. 系统计算玩家伤害 (基础攻击 - 防御/2)
3. 10% 概率触发暴击 (2 倍伤害)
4. 对怪物应用伤害
5. 检查怪物是否死亡
   - 若死亡：玩家胜利，获得经验，战斗结束
   - 若存活：怪物反击，返回双方伤害
```

**命令示例 (战斗胜利):**
```
> attack
You attack the Goblin with a CRITICAL HIT and deal 16 damage!
Goblin is on the verge of death!

*** VICTORY! ***
You have defeated the Goblin!
You gained 10 experience points!
[Level Up!] You are now level 2!
HP and MP restored, Attack and Defense increased!
```

**命令示例 (战斗继续):**
```
> attack
You attack the Goblin and deal 7 damage!
Goblin is bleeding and breathing heavily.

The Goblin attacks you and deals 4 damage!
You barely feel the wound.
```

**扩展流程:**
- 🔶 技能系统 (魔法/战技)
- 🔶 装备耐久度
- 🔶 战斗日志详细化

---

### UC-COMBAT-03: 逃跑

| 属性 | 描述 |
|------|------|
| **类型** | ✅ Include (主干) |
| **参与者** | 玩家、CombatManager、Session |
| **前置条件** | 玩家处于战斗状态 |
| **后置条件** | 可能成功逃脱或失败被反击 |

**主流程:**
```
1. 玩家输入命令：flee
2. 系统生成 0-100 随机数
3. 若随机数 < 50 (50% 成功率)
   - 战斗结束，玩家传送回起始大厅
   - 返回成功消息
4. 若随机数 >= 50
   - 怪物反击一次
   - 返回失败消息
```

**命令示例 (成功):**
```
> flee
You manage to escape from the fight!
You arrive at Grand Starting Hall, panting heavily.
```

**命令示例 (失败):**
```
> flee
You try to flee, but the monster blocks your path!
The Goblin attacks you and deals 5 damage!
You barely feel the wound.
```

**扩展流程:**
- 🔶 逃跑成功率与属性相关
- 🔶 连续逃跑成功率递减
- 🔶 某些 Boss 战无法逃跑

---

### UC-COMBAT-04: 战斗失败复活

| 属性 | 描述 |
|------|------|
| **类型** | ✅ Include (主干) |
| **参与者** | 玩家、CombatManager、Session |
| **前置条件** | 玩家 HP 降至 0 或以下 |
| **后置条件** | 玩家被传送回起始点 |

**主流程:**
```
1. 系统检测到玩家 HP <= 0
2. 设置战斗状态为 CombatLost
3. 返回失败消息
4. 传送玩家到起始大厅
5. 玩家 HP/MP 恢复 (隐含)
```

**命令示例:**
```
*** DEFEAT ***
You have been defeated by the Orc Berserker...
You black out and wake up back at the Starting Hall.
```

**扩展流程:**
- 🔶 死亡掉落物品/金币
- 🔶 复活冷却时间
- 🔶 墓地系统 (可选择复活点)
- 🔶 经验值惩罚

---

### UC-COMBAT-05: 查看战斗状态

| 属性 | 描述 |
|------|------|
| **类型** | 🔶 Extend (扩展) |
| **参与者** | 玩家、CombatManager |
| **前置条件** | 玩家处于战斗状态 |
| **后置条件** | 显示当前战斗信息 |

**主流程:**
```
1. 玩家输入命令：combat 或 status
2. 系统返回当前怪物 HP 状态
```

**命令示例:**
```
> combat
You are fighting a Goblin.
Goblin is bleeding and breathing heavily.
```

**扩展流程:**
- 🔶 显示详细战斗统计
- 🔶 显示 Buff/Debuff 状态
- 🔶 战斗回合计数

---

### UC-COMBAT-06: 使用物品/技能

| 属性 | 描述 |
|------|------|
| **类型** | 🔶 Extend (扩展) |
| **参与者** | 玩家、Player、CombatManager |
| **前置条件** | 玩家处于战斗状态，拥有可用物品 |
| **后置条件** | 物品效果生效 |

**主流程:**
```
1. 玩家输入命令：use <物品名>
2. 系统检查背包中是否有该物品
3. 应用物品效果 (治疗/增益等)
4. 从背包移除消耗品
5. 怪物回合反击
```

**命令示例:**
```
> use Health Potion
You drink the Health Potion and recover 50 HP!
The Goblin attacks you and deals 3 damage!
```

**扩展流程:**
- 🔶 技能冷却时间
- 🔶 魔法值消耗
- 🔶 物品使用次数限制

---

## 4. 保存模块 (Save System)

### 功能概述
管理玩家数据的持久化存储，包括保存、加载、删除存档。

### 核心类
- `SaveManager` - 存档管理单例
- `PlayerData` - 玩家数据结构
- `Item` - 物品数据结构

---

### UC-SAVE-01: 手动保存游戏

| 属性 | 描述 |
|------|------|
| **类型** | ✅ Include (主干) |
| **参与者** | 玩家、SaveManager、Player |
| **前置条件** | 玩家已设置有效名称 |
| **后置条件** | 玩家数据保存到磁盘文件 |

**主流程:**
```
1. 玩家输入命令：save
2. 系统检查玩家是否有有效名称 (非 "Stranger")
3. 生成存档文件名：<角色名>.sav
4. 将 Player 数据序列化为 PlayerData
5. 将 PlayerData 转换为 JSON 格式
6. 写入文件到 saves/ 目录
7. 返回保存成功消息
```

**命令示例:**
```
> save
[Save] Game saved successfully as 'Arthur.sav'
Your progress has been preserved!
```

**异常流程:**
- 玩家未设置名称 → 提示 "You need to set a name first!"
- 文件写入失败 → 提示 "Failed to save game."

**存档文件示例 (saves/Arthur.sav):**
```json
{
  "name": "Arthur",
  "description": "A mysterious adventurer",
  "location": "hall",
  "level": 5,
  "exp": 250,
  "max_hp": 180,
  "current_hp": 180,
  "max_mp": 90,
  "current_mp": 90,
  "attack": 18,
  "defense": 9,
  "gold": 100,
  "inventory": [...]
}
```

**扩展流程:**
- 🔶 多存档槽位
- 🔶 存档压缩
- 🔶 存档加密

---

### UC-SAVE-02: 加载存档

| 属性 | 描述 |
|------|------|
| **类型** | ✅ Include (主干) |
| **参与者** | 玩家、SaveManager、Player |
| **前置条件** | 存在有效的存档文件 |
| **后置条件** | 玩家数据被存档数据替换 |

**主流程:**
```
1. 玩家输入命令：load [角色名]
2. 若未指定角色名，使用当前角色名
3. 生成存档文件名并检查是否存在
4. 读取 JSON 文件
5. 反序列化为 PlayerData
6. 应用数据到 Player 对象
7. 更新玩家位置到存档中的房间
8. 返回加载成功消息
```

**命令示例:**
```
> load Arthur
[Load] Welcome back, Arthur!
Loaded character:
  Level: 5
  HP: 180/180
  MP: 90/90
  Gold: 100
  Items: 3

You are now at Grand Starting Hall.
```

**异常流程:**
- 存档不存在 → 提示 "No save file found for 'Arthur'"
- 存档损坏 → 提示 "Failed to load save file. It may be corrupted."

**扩展流程:**
- 🔶 加载前预览存档信息
- 🔶 跨服务器加载
- 🔶 存档版本兼容

---

### UC-SAVE-03: 查看存档列表

| 属性 | 描述 |
|------|------|
| **类型** | ✅ Include (主干) |
| **参与者** | 玩家、SaveManager |
| **前置条件** | 无 |
| **后置条件** | 显示所有可用存档 |

**主流程:**
```
1. 玩家输入命令：saves
2. 系统扫描 saves/ 目录下所有 .sav 文件
3. 提取文件名 (不含扩展名)
4. 返回格式化列表
```

**命令示例:**
```
> saves
====== Saved Characters ======
  - Arthur
  - Lancelot
  - Merlin
==============================
Use 'load <name>' to load a character.
Use 'delete <name>' to delete a save.
```

**扩展流程:**
- 🔶 显示存档详细信息 (等级/位置/游戏时间)
- 🔶 存档排序 (按时间/等级)
- 🔶 存档缩略图

---

### UC-SAVE-04: 删除存档

| 属性 | 描述 |
|------|------|
| **类型** | ✅ Include (主干) |
| **参与者** | 玩家、SaveManager |
| **前置条件** | 存档文件存在 |
| **后置条件** | 存档文件被永久删除 |

**主流程:**
```
1. 玩家输入命令：delete <角色名>
2. 生成存档文件名并检查是否存在
3. 从磁盘删除文件
4. 返回删除成功消息
```

**命令示例:**
```
> delete OldCharacter
[Delete] Save file 'OldCharacter.sav' has been deleted.
```

**异常流程:**
- 存档不存在 → 提示 "No save file found for 'OldCharacter'"

**扩展流程:**
- 🔶 删除确认提示 ("Are you sure? y/n")
- 🔶 回收站机制 (可恢复)
- 🔶 批量删除

---

### UC-SAVE-05: 退出自动保存

| 属性 | 描述 |
|------|------|
| **类型** | ✅ Include (主干) |
| **参与者** | 玩家、SaveManager、Session |
| **前置条件** | 玩家已设置有效名称 |
| **后置条件** | 玩家数据保存，会话关闭 |

**主流程:**
```
1. 玩家输入命令：quit
2. 系统自动生成存档文件名
3. 保存玩家数据到文件
4. 向同房间玩家广播离开消息
5. 关闭会话连接
6. 返回告别消息
```

**命令示例:**
```
> quit
[Auto-Save] Game saved as 'Arthur.sav'

Thank you for playing, Arthur! See you again.
Quitting game...
Arthur has left the game.
```

**扩展流程:**
- 🔶 退出前确认
- 🔶 定时自动保存
- 🔶 关键事件触发保存 (升级/Boss 战前后)

---

### UC-SAVE-06: 存档导入/导出

| 属性 | 描述 |
|------|------|
| **类型** | 🔶 Extend (扩展) |
| **参与者** | 玩家、SaveManager |
| **前置条件** | 存在存档文件 |
| **后置条件** | 存档文件被导入或导出 |

**主流程:**
```
1. 玩家输入命令：export <角色名> 或 import <文件名>
2. 系统复制存档文件到指定位置
3. 返回操作结果
```

**命令示例:**
```
> export Arthur
[Export] Save file 'Arthur.sav' exported to 'exports/Arthur_20260401.sav'
```

**扩展流程:**
- 🔶 云存档同步
- 🔶 存档分享码
- 🔶 跨平台存档兼容

---

## 附录：功能优先级矩阵

| 模块 | 用例 ID | 用例名称 | 优先级 | 工作量估算 |
|------|---------|----------|--------|------------|
| **用户服务** | UC-USER-01 | 玩家登录游戏 | P0 | 2h |
| | UC-USER-02 | 设置角色名称 | P0 | 1h |
| | UC-USER-03 | 查看在线玩家 | P1 | 1h |
| | UC-USER-04 | 查看玩家状态 | P0 | 1h |
| | UC-USER-05 | 房间聊天 | P1 | 1h |
| | UC-USER-06 | 查看背包物品 | P1 | 2h |
| **地图** | UC-MAP-01 | 查看房间信息 | P0 | 2h |
| | UC-MAP-02 | 房间移动 | P0 | 3h |
| | UC-MAP-03 | 与 NPC 交互 | P2 | 4h |
| | UC-MAP-04 | 发现隐藏房间 | P2 | 4h |
| **战斗** | UC-COMBAT-01 | 发起战斗 | P0 | 3h |
| | UC-COMBAT-02 | 玩家攻击 | P0 | 4h |
| | UC-COMBAT-03 | 逃跑 | P1 | 2h |
| | UC-COMBAT-04 | 战斗失败复活 | P0 | 2h |
| | UC-COMBAT-05 | 查看战斗状态 | P2 | 1h |
| | UC-COMBAT-06 | 使用物品/技能 | P2 | 4h |
| **保存** | UC-SAVE-01 | 手动保存游戏 | P0 | 3h |
| | UC-SAVE-02 | 加载存档 | P0 | 3h |
| | UC-SAVE-03 | 查看存档列表 | P1 | 1h |
| | UC-SAVE-04 | 删除存档 | P1 | 1h |
| | UC-SAVE-05 | 退出自动保存 | P0 | 2h |
| | UC-SAVE-06 | 存档导入/导出 | P2 | 3h |

**优先级说明:**
- **P0**: MVP 核心功能，必须实现
- **P1**: 重要增强功能，建议实现
- **P2**: 扩展功能，可选实现

---

## 修订历史

| 版本 | 日期 | 作者 | 变更说明 |
|------|------|------|----------|
| 1.0 | 2026-04-01 | - | 初始版本，基于现有代码分析 |
