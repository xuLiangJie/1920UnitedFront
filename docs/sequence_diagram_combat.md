# MUD 游戏战斗攻击判定顺序图

**版本:** 1.0  
**日期:** 2026-04-01  
**依据:** 基于现有代码逆推 (Combat.cpp, Monster.cpp, Player.cpp)

---

## 1. 玩家攻击完整流程 (Sequence Diagram)

```plantuml
@startuml Player_Attack_Sequence_Diagram

title 玩家攻击判定完整流程

autonumber

participant "玩家" as Player
participant "Session" as Session
participant "CommandHandler" as CmdHandler
participant "CombatManager" as CombatMgr
participant "Monster" as Monster
participant "Player(HP)" as PlayerHP

== 阶段 1: 玩家输入攻击命令 ==

Player -> Session : 输入 "attack" 或 "kill <monster>"
Session -> CmdHandler : handleCommand(session, "attack")
CmdHandler -> CmdHandler : 解析命令，识别为 cmdAttack()

alt 检查战斗状态
    CmdHandler -> CombatMgr : isInCombat()
    CombatMgr --> CmdHandler : false
    CmdHandler --> Session : "You are not in combat..."
    Session --> Player : 返回错误消息
else 处于战斗状态
    CmdHandler -> CombatMgr : isInCombat()
    CombatMgr --> CmdHandler : true
    CmdHandler -> CombatMgr : playerAttack(session)
    
    == 阶段 2: 玩家攻击判定 ==
    
    activate CombatMgr
    
    CombatMgr -> CombatMgr : 检查 state_ == InCombat && monster.isAlive()
    
    alt 战斗状态无效
        CombatMgr --> CmdHandler : result.message = "Not in combat"
    else 战斗状态有效
        CombatMgr -> CombatMgr : 生成玩家攻击基数 (8 + rand() % 4)
        CombatMgr -> Monster : getDefense()
        Monster --> CombatMgr : 防御值 (例如：0)
        
        CombatMgr -> CombatMgr : 计算基础伤害\nmax(1, player_attack - defense/2)
        
        alt 暴击判定 (10% 概率)
            CombatMgr -> CombatMgr : rand() % 100 < 10
            CombatMgr -> CombatMgr : damage *= 2 (暴击！)
        else 普通命中
            CombatMgr -> CombatMgr : 保持原伤害
        end
        
        CombatMgr -> Monster : takeDamage(damage)
        activate Monster
        Monster -> Monster : actual_damage = max(1, damage - defense)\ncurrent_hp -= actual_damage
        Monster --> CombatMgr : 伤害已应用
        deactivate Monster
        
        CombatMgr -> CombatMgr : 记录 result.damageDealt
        CombatMgr -> CombatMgr : 构建攻击消息
        
        == 阶段 3: 检查怪物存活状态 ==
        
        CombatMgr -> Monster : isDead() (current_hp <= 0)
        
        alt 怪物死亡 (胜利)
            Monster --> CombatMgr : true
            CombatMgr -> CombatMgr : result.playerWon = true\nresult.expGained = exp_reward
            
            CombatMgr -> CombatMgr : 构建胜利消息
            
            CombatMgr -> Session : endCombat(session, true)
            activate Session
            
            Session -> Session : 获取当前房间
            Session -> Session : (可选) 从房间移除怪物
            Session --> CombatMgr : 完成
            
            CombatMgr -> CombatMgr : state_ = CombatWon → NotInCombat
            
            CombatMgr --> CmdHandler : result (含胜利消息)
            deactivate Session
            
        else 怪物存活 (继续战斗)
            Monster --> CombatMgr : false
            
            CombatMgr -> CombatMgr : 调用 monsterAttack(session)
            
            == 阶段 4: 怪物反击 ==
            
            CombatMgr -> CombatMgr : 检查战斗状态
            CombatMgr -> Monster : attack()
            activate Monster
            Monster -> Monster : calculateDamage(attack_)\nvariance = base_attack / 5\nreturn min_dmg + rand() % range
            Monster --> CombatMgr : 怪物伤害 (例如：5)
            deactivate Monster
            
            CombatMgr -> CombatMgr : 玩家防御减免\nactual_damage = max(1, damage - player_defense)
            
            CombatMgr -> PlayerHP : 计算虚拟 HP 状态\nplayer_hp = 100 - actual_damage
            
            alt 玩家 HP 状态分支
                player_hp > 80 : "You barely feel the wound."
                player_hp > 60 : "You take a minor hit."
                player_hp > 40 : "You are starting to feel the pain."
                player_hp > 20 : "You are badly hurt!"
                else : "You are on the brink of death!"
            end
            
            CombatMgr -> CombatMgr : 记录 result.damageReceived
            
            alt 玩家死亡判定 (简化版)
                damageReceived >= 100 : 玩家战败
                CombatMgr -> Session : endCombat(session, false)
                activate Session
                Session -> Session : state_ = CombatLost
                Session -> Session : 广播失败消息
                Session -> Session : setCurrentRoom("hall")\n(传送回起始大厅)
                Session --> CombatMgr : 完成
                deactivate Session
                CombatMgr -> CombatMgr : state_ = NotInCombat
            else 玩家存活
                CombatMgr -> CombatMgr : 战斗继续
            end
            
            CombatMgr --> CmdHandler : result (含双方消息)
        end
    end
    
    deactivate CombatMgr
    
    == 阶段 5: 返回结果给玩家 ==
    
    CmdHandler -> CmdHandler : 检查是否升级\nif expGained > 0: player.addExp()
    
    alt 升级发生
        CmdHandler -> PlayerHP : addExp(amount)
        activate PlayerHP
        PlayerHP -> PlayerHP : exp_ += amount
        PlayerHP -> PlayerHP : while exp >= level*100: level++\nmax_hp += 20, attack += 2...
        PlayerHP --> CmdHandler : 升级完成
        deactivate PlayerHP
        CmdHandler --> Session : 含升级消息
    else 无升级
        CmdHandler --> Session : 战斗结果消息
    end
    
    Session --> Player : 显示完整战斗结果
end

@enduml
```

---

## 2. 核心判定分支详解 (Alt 片段分解)

### 2.1 暴击判定分支

```plantuml
@startuml Crit_Check

title 暴击判定逻辑

participant "CombatManager" as CM

CM -> CM : 基础伤害计算完成

alt 暴击判定 (10% 概率)
    CM -> CM : roll = rand() % 100
    CM -> CM : roll < 10 ?
    
    alt 暴击命中
        CM -> CM : damage *= 2
        CM -> CM : 标记 isCrit = true
        note right: 最终伤害 = 基础伤害 × 2
    else 暴击未命中
        CM -> CM : 保持原伤害
        CM -> CM : isCrit = false
        note right: 最终伤害 = 基础伤害
    end
else 跳过暴击
    note right: 非暴击流程
end

CM -> CM : 构建消息\nif (isCrit) "CRITICAL HIT"

@enduml
```

---

### 2.2 怪物存活判定分支

```plantuml
@startuml Monster_Death_Check

title 怪物死亡判定逻辑

participant "CombatManager" as CM
participant "Monster" as M

CM -> M : isDead()
activate M

M -> M : current_hp <= 0 ?

alt 怪物已死亡
    M --> CM : true
    note right: 战斗胜利流程\n- 设置 playerWon = true\n- 计算经验奖励\n- 结束战斗\n- 可能触发升级
    CM -> CM : result.playerWon = true
    CM -> CM : result.expGained = exp_reward
    CM -> CM : endCombat(session, true)
else 怪物存活
    M --> CM : false
    note right: 战斗继续流程\n- 怪物反击\n- 玩家承受伤害\n- 检查玩家是否死亡
    CM -> CM : monsterAttack(session)
end

deactivate M

@enduml
```

---

### 2.3 玩家受伤状态分支

```plantuml
@startuml Player_HP_Status

title 玩家受伤状态反馈

participant "CombatManager" as CM

CM -> CM : actual_damage = 怪物伤害 - 玩家防御

CM -> CM : player_hp = 100 - actual_damage\n(简化版，不追踪实际 HP)

alt HP > 80 (轻伤)
    CM -> CM : "You barely feel the wound."
else HP > 60 (小伤)
    CM -> CM : "You take a minor hit."
else HP > 40 (中度伤)
    CM -> CM : "You are starting to feel the pain."
else HP > 20 (重伤)
    CM -> CM : "You are badly hurt! Be careful!"
else HP <= 20 (濒死)
    CM -> CM : "You are on the brink of death! Flee if you can!"
end

CM -> CM : 将状态消息附加到战斗结果

@enduml
```

---

### 2.4 玩家死亡/复活分支

```plantuml
@startuml Player_Death_Respawn

title 玩家死亡与复活流程

participant "CombatManager" as CM
participant "Session" as S
participant "World" as W

CM -> CM : 检查 damageReceived >= 100\n(简化版死亡判定)

alt 玩家死亡
    CM -> S : endCombat(session, false)
    activate S
    
    S -> S : state_ = CombatLost
    
    S -> S : 构建失败消息\n"*** DEFEAT ***"\n"You have been defeated by..."
    S -> S : "You black out and wake up\nback at the Starting Hall."
    
    S -> W : setCurrentRoom("hall")
    activate W
    W -> W : current_room_id_ = "hall"
    W -> W : player.location_ = "hall"
    W --> S : 位置更新完成
    deactivate W
    
    S --> CM : 完成
    deactivate S
    
    CM -> CM : state_ = NotInCombat
    
    note right: 玩家被传送回\n起始大厅 (hall)
else 玩家存活
    CM -> CM : 战斗继续
    note right: 玩家可以继续\n攻击或逃跑
end

@enduml
```

---

### 2.5 升级判定分支

```plantuml
@startuml Level_Up_Check

title 升级判定流程

participant "CommandHandler" as CH
participant "Player" as P

CH -> CH : 检查 result.expGained > 0

alt 获得经验
    CH -> P : addExp(amount)
    activate P
    
    P -> P : exp_ += amount
    P -> P : expNeeded = level_ * 100
    
    loop 检查是否满足升级条件
        alt exp >= expNeeded
            P -> P : level_++
            P -> P : exp_ -= expNeeded
            P -> P : max_hp_ += 20
            P -> P : current_hp_ = max_hp_ (满血)
            P -> P : max_mp_ += 10
            P -> P : current_mp_ = max_mp_ (满魔)
            P -> P : attack_ += 2
            P -> P : defense_ += 1
            P -> P : expNeeded = level_ * 100
            note right: 升级！\n全属性提升\nHP/MP 回满
        else exp < expNeeded
            P -> P : 不升级
        end
    end
    
    P --> CH : 返回是否升级
    deactivate P
    
    alt 升级发生
        CH -> CH : 构建升级消息\n"[Level Up!] You are now level X!"\n"HP and MP restored..."
    else 未升级
        CH -> CH : 仅显示战斗结果
    end
else 无经验获得
    note right: 逃跑或战斗失败\n无经验奖励
end

@enduml
```

---

## 3. 怪物反击流程详解

```plantuml
@startuml Monster_Counter_Attack

title 怪物反击完整流程

autonumber

participant "CombatManager" as CM
participant "Monster" as M
participant "Player" as P

== 怪物反击阶段 ==

CM -> CM : 检查 state_ == InCombat && monster.isAlive()

alt 战斗状态有效
    CM -> M : attack()
    activate M
    
    M -> M : base_attack = attack_ (例如：5)
    M -> M : calculateDamage(base_attack)
    
    M -> M : variance = base_attack / 5\n(±20% 伤害波动)
    M -> M : min_dmg = base_attack - variance
    M -> M : max_dmg = base_attack + variance
    
    M -> M : actual_damage = min_dmg + rand() % range
    note right: 伤害随机化\n增加战斗不确定性
    
    M --> CM : 返回伤害值
    deactivate M
    
    CM -> CM : player_defense = 2 (简化值)
    CM -> CM : final_damage = max(1, actual_damage - player_defense)
    note right: 至少造成 1 点伤害\n防止无敌状态
    
    CM -> CM : 记录 result.damageDealt
    
    CM -> CM : 构建受伤消息
    CM -> CM : 根据玩家 HP 百分比\n选择对应状态描述
    
    CM -> CM : 返回 CombatResult
else 战斗状态无效
    CM -> CM : 返回空结果
end

@enduml
```

---

## 4. 完整战斗回合时序 (多轮攻击)

```plantuml
@startuml Full_Combat_Rounds

title 完整战斗回合流程 (多轮攻击)

autonumber

participant "玩家" as Player
participant "Session" as S
participant "CombatManager" as CM
participant "Monster" as M

== 战斗开始 ==

Player -> S : kill Goblin
S -> CM : startCombat(session, Goblin)
CM -> S : "====== COMBAT STARTED ======"
S --> Player : 显示战斗开始消息

== 第 1 回合 ==

Player -> S : attack
S -> CM : playerAttack(session)

alt 第 1 回合：玩家攻击
    CM -> M : takeDamage(8)
    M --> CM : HP: 25 → 17
    CM -> CM : 怪物存活
    CM -> M : attack()
    M --> CM : 伤害 5
    CM -> CM : 玩家受伤 (轻伤)
    CM --> S : 返回双方消息
    S --> Player : "You attack... 8 damage"\n"Goblin attacks... 5 damage"
end

== 第 2 回合 ==

Player -> S : attack
S -> CM : playerAttack(session)

alt 第 2 回合：暴击！
    CM -> CM : 暴击判定成功 (roll=5 < 10)
    CM -> M : takeDamage(16) (暴击×2)
    M --> CM : HP: 17 → 1
    CM -> CM : 怪物存活 (HP>0)
    CM -> M : attack()
    M --> CM : 伤害 4
    CM -> CM : 玩家受伤
    CM --> S : 返回双方消息
    S --> Player : "CRITICAL HIT! 16 damage"\n"Goblin attacks... 4 damage"
end

== 第 3 回合 ==

Player -> S : attack
S -> CM : playerAttack(session)

alt 第 3 回合：击杀！
    CM -> M : takeDamage(9)
    M --> CM : HP: 1 → -8 (死亡)
    CM -> M : isDead()
    M --> CM : true
    CM -> CM : result.playerWon = true
    CM -> CM : result.expGained = 10
    CM -> S : endCombat(session, true)
    CM --> S : 返回胜利消息
    S --> Player : "*** VICTORY! ***"\n"You gained 10 EXP!"
end

== 战斗结束 ==

note right: 战斗状态重置\nstate_ = NotInCombat\n怪物从房间移除

@enduml
```

---

## 5. 伤害计算公式汇总

### 5.1 玩家对怪物伤害

```
┌─────────────────────────────────────────────────────────┐
│  玩家伤害计算流程                                        │
├─────────────────────────────────────────────────────────┤
│  1. 基础攻击 = 8 + rand() % 4          (8-11 范围)      │
│                                                         │
│  2. 基础伤害 = max(1, 基础攻击 - 怪物防御/2)            │
│                                                         │
│  3. 暴击判定 = rand() % 100 < 10       (10% 概率)       │
│     - 是：最终伤害 = 基础伤害 × 2                       │
│     - 否：最终伤害 = 基础伤害                           │
│                                                         │
│  4. 怪物实际受伤 = max(1, 最终伤害 - 怪物防御)          │
│                                                         │
│  5. 怪物 HP = max(0, 当前 HP - 实际受伤)                │
└─────────────────────────────────────────────────────────┘
```

### 5.2 怪物对玩家伤害

```
┌─────────────────────────────────────────────────────────┐
│  怪物伤害计算流程                                        │
├─────────────────────────────────────────────────────────┤
│  1. 基础攻击 = 怪物.attack_ (根据难度等级)              │
│                                                         │
│  2. 伤害波动 = 基础攻击 / 5            (±20% 浮动)      │
│     最小伤害 = 基础攻击 - 波动                          │
│     最大伤害 = 基础攻击 + 波动                          │
│     实际攻击 = 最小伤害 + rand() % (范围)               │
│                                                         │
│  3. 玩家防御减免 = 2 (简化固定值)                       │
│                                                         │
│  4. 最终伤害 = max(1, 实际攻击 - 玩家防御)              │
│                                                         │
│  5. 玩家虚拟 HP = 100 - 最终伤害       (仅用于反馈)     │
└─────────────────────────────────────────────────────────┘
```

### 5.3 怪物难度属性对照

| 难度 | HP 范围 | 攻击范围 | 防御 | 经验 |
|------|---------|----------|------|------|
| Easy (哥布林/野狼) | 20-30 | 3-6 | 0 | 10 |
| Normal (骷髅战士) | 40-60 | 6-10 | 1 | 25 |
| Hard (兽人狂战士) | 80-110 | 10-15 | 2 | 50 |
| Boss (幼龙) | 150-200 | 15-23 | 4 | 150 |

---

## 6. 代码审查发现的问题

### 6.1 缺失的判定分支

| 问题 ID | 描述 | 当前状态 | 建议 |
|---------|------|----------|------|
| M-01 | **闪避判定** | ❌ 未实现 | 添加命中率/闪避率属性 |
| M-02 | **格挡/招架** | ❌ 未实现 | 添加防御动作选项 |
| M-03 | **玩家真实 HP 追踪** | ⚠️ 简化版 | 使用 Player.current_hp_ |
| M-04 | **多目标战斗** | ❌ 未实现 | 支持群体战斗 |
| M-05 | **技能/魔法攻击** | ❌ 未实现 | 添加技能系统 |

### 6.2 硬编码问题

```cpp
// Combat.cpp 中的硬编码
int player_attack = 8 + rand() % 4;  // ⚠️ 应使用 Player.attack_
int player_defense = 2;              // ⚠️ 应使用 Player.defense_
if (result.damageReceived >= 100)    // ⚠️ 应使用 Player.current_hp_ <= 0
```

### 6.3 建议的扩展判定

```plantuml
@startuml Extended_Combat_Checks

title 建议扩展的战斗判定

start

:玩家输入 attack;

if (战斗状态？) then (否)
    :返回错误;
    stop
else (是)
    if (命中率判定？) then (未命中)
        :攻击 Miss!\n(显示"被闪避");
    else (命中)
        if (暴击判定？) then (是)
            :伤害 × 2;
        else (否)
            :正常伤害;
        endif
        
        :应用伤害到怪物;
        
        if (怪物死亡？) then (是)
            :胜利!\n获得经验;
            stop
        else (否)
            if (怪物反击命中？) then (是)
                if (玩家格挡？) then (成功)
                    :伤害减免 50%;
                else (失败)
                    :承受全额伤害;
                endif
                
                if (玩家死亡？) then (是)
                    :失败!\n传送回大厅;
                    stop
                else (否)
                    :显示受伤状态;
                endif
            else (否)
                :怪物攻击未命中;
            endif
        endif
    endif
endif

:返回战斗结果;

@enduml
```

---

## 7. 总结

### 7.1 当前实现的判定分支

```
玩家攻击
├── 战斗状态检查 ✓
│   ├── 有效 → 继续
│   └── 无效 → 返回错误
│
├── 伤害计算 ✓
│   ├── 基础伤害 = 攻击 - 防御/2
│   └── 最小伤害 = 1
│
├── 暴击判定 ✓
│   ├── 成功 (10%) → 伤害 ×2
│   └── 失败 → 正常伤害
│
├── 怪物存活检查 ✓
│   ├── 死亡 → 胜利，获得经验
│   └── 存活 → 怪物反击
│
└── 怪物反击 ✓
    ├── 伤害计算 (±20% 波动)
    ├── 防御减免
    ├── 玩家状态反馈 (5 档 HP 状态)
    └── 玩家死亡检查 (简化版)
        ├── 死亡 → 传送回大厅
        └── 存活 → 战斗继续
```

### 7.2 建议优先实现的功能

| 优先级 | 功能 | 理由 |
|--------|------|------|
| P0 | 使用 Player 真实 HP | 修复当前简化版逻辑 |
| P0 | 移除硬编码属性 | 提高代码可维护性 |
| P1 | 闪避/命中判定 | 增加战斗策略性 |
| P1 | 防御动作 (格挡/招架) | 增加玩家选择 |
| P2 | 技能系统 | 丰富战斗玩法 |

---

**文档版本历史:**

| 版本 | 日期 | 变更说明 |
|------|------|----------|
| 1.0 | 2026-04-01 | 基于现有代码逆推初始版本 |









```mermaid

sequenceDiagram
    autonumber
    title 玩家攻击判定完整流程

    participant Player as 玩家
    participant Session
    participant CmdHandler as CommandHandler
    participant CombatMgr as CombatManager
    participant Monster
    participant PlayerHP as PlayerHP

    Note over Player,Session: 阶段 1: 玩家输入攻击命令

    Player->>Session: 输入 "attack" 或 "kill <monster>"
    Session->>CmdHandler: handleCommand(session, "attack")
    CmdHandler->>CmdHandler: 解析命令，识别为 cmdAttack()

    alt 检查战斗状态
        CmdHandler->>CombatMgr: isInCombat()
        CombatMgr-->>CmdHandler: false
        CmdHandler-->>Session: You are not in combat...
        Session-->>Player: 返回错误消息
    else 处于战斗状态
        CmdHandler->>CombatMgr: isInCombat()
        CombatMgr-->>CmdHandler: true
        CmdHandler->>CombatMgr: playerAttack(session)
    end

    Note over CombatMgr,Monster: 阶段 2: 玩家攻击判定

    activate CombatMgr
    CombatMgr->>CombatMgr: 检查战斗状态有效性

    alt 战斗状态无效
        CombatMgr-->>CmdHandler: Not in combat
    else 战斗状态有效
        CombatMgr->>CombatMgr: 生成玩家攻击基数
        CombatMgr->>Monster: getDefense()
        Monster-->>CombatMgr: 防御值
        CombatMgr->>CombatMgr: 计算基础伤害
        
        alt 暴击判定
            CombatMgr->>CombatMgr: 10%概率判定
            CombatMgr->>CombatMgr: damage *= 2
            Note right of CombatMgr: 暴击！伤害翻倍
        else 普通命中
            CombatMgr->>CombatMgr: 保持原伤害
        end
        
        CombatMgr->>Monster: takeDamage(damage)
        activate Monster
        Monster->>Monster: 扣除生命值
        Monster-->>CombatMgr: 伤害已应用
        deactivate Monster
    end

    Note over CombatMgr,Session: 阶段 3: 检查怪物存活状态

    CombatMgr->>Monster: isDead()
    
    alt 怪物死亡
        Monster-->>CombatMgr: true
        CombatMgr->>CombatMgr: 计算经验奖励
        CombatMgr->>Session: endCombat(session, true)
        activate Session
        Session->>Session: 从房间移除怪物
        Session-->>CombatMgr: 完成
        deactivate Session
        CombatMgr->>CombatMgr: 重置战斗状态
        CombatMgr-->>CmdHandler: 返回胜利结果
    else 怪物存活
        Monster-->>CombatMgr: false
        CombatMgr->>CombatMgr: 调用怪物反击
        
        Note over CombatMgr,PlayerHP: 阶段 4: 怪物反击
        
        CombatMgr->>Monster: attack()
        activate Monster
        Monster->>Monster: 计算伤害值
        Monster-->>CombatMgr: 怪物伤害
        deactivate Monster
        
        CombatMgr->>CombatMgr: 玩家防御减免
        CombatMgr->>PlayerHP: 计算剩余血量
        
        alt 高血量状态
            PlayerHP-->>CombatMgr: HP > 80%
            CombatMgr->>CombatMgr: You barely feel the wound.
        else 中等血量状态
            PlayerHP-->>CombatMgr: HP > 60%
            CombatMgr->>CombatMgr: You take a minor hit.
        else 较低血量状态
            PlayerHP-->>CombatMgr: HP > 40%
            CombatMgr->>CombatMgr: You are starting to feel the pain.
        else 低血量状态
            PlayerHP-->>CombatMgr: HP > 20%
            CombatMgr->>CombatMgr: You are badly hurt!
        else 濒死状态
            PlayerHP-->>CombatMgr: HP <= 20%
            CombatMgr->>CombatMgr: You are on the brink of death!
        end
        
        alt 玩家死亡
            CombatMgr->>CombatMgr: 判定玩家死亡
            CombatMgr->>Session: endCombat(session, false)
            activate Session
            Session->>Session: 传送回起始大厅
            Session->>Session: 广播失败消息
            Session-->>CombatMgr: 完成
            deactivate Session
            CombatMgr->>CombatMgr: 重置战斗状态
        else 玩家存活
            CombatMgr->>CombatMgr: 战斗继续
        end
        
        CombatMgr-->>CmdHandler: 返回战斗结果
    end
    
    deactivate CombatMgr

    Note over CmdHandler,Player: 阶段 5: 返回结果给玩家

    CmdHandler->>CmdHandler: 检查是否升级
    
    alt 升级发生
        CmdHandler->>PlayerHP: addExp(amount)
        activate PlayerHP
        PlayerHP->>PlayerHP: 增加经验值
        PlayerHP->>PlayerHP: 检查升级条件
        PlayerHP->>PlayerHP: 提升等级和属性
        PlayerHP-->>CmdHandler: 升级完成
        deactivate PlayerHP
        CmdHandler-->>Session: 返回升级消息
    else 无升级
        CmdHandler-->>Session: 返回战斗结果消息
    end
    
    Session-->>Player: 显示完整战斗结果
```
