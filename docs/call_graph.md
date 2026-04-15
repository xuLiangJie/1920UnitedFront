# MUD 游戏代码调用关系图

## 1. 模块架构图

```mermaid
graph TB
    subgraph Client["客户端 (mud_client.exe)"]
        main_client["main()"]
        MudClient["MudClient"]
        net_client["Network"]
    end

    subgraph Server["服务器 (mud_server.exe)"]
        main_server["main()"]
        MudServer["MudServer"]
        Session["Session"]
        World["World"]
        CommandHandler["CommandHandler"]
        CombatManager["CombatManager"]
        SaveManager["SaveManager"]
        net_server["Network"]
    end

    main_client --> MudClient
    MudClient --> net_client
    
    main_server --> MudServer
    MudServer --> Session
    MudServer --> World
    MudServer --> net_server
    Session --> CommandHandler
    Session --> CombatManager
    
    MudClient -.->|TCP 连接 | net_server
```

---

## 2. 服务器核心调用关系

```mermaid
graph LR
    main_server["main()"] --> MudServer_start["MudServer::start()"]
    
    MudServer_start --> net_init["network::init()"]
    MudServer_start --> net_create["network::create_tcp_socket()"]
    MudServer_start --> net_bind["network::bind_socket()"]
    MudServer_start --> net_listen["network::listen_socket()"]
    
    MudServer_start --> accept_loop["accept 循环"]
    accept_loop --> net_accept["network::accept_socket()"]
    net_accept --> Session_new["new Session"]
    Session_new --> Session_start["Session::start()"]
    
    Session_start --> readLoop["readLoop 线程"]
    Session_start --> writeLoop["writeLoop 线程"]
    
    readLoop --> net_recv["network::recv_data()"]
    readLoop --> cmd_handle["CommandHandler::handleCommand()"]
    
    writeLoop --> net_send["network::send_data()"]
```

---

## 3. 命令处理调用关系

```mermaid
graph TB
    handleCommand["CommandHandler::handleCommand()"] --> parseInput["parseInput()"]
    parseInput --> cmd_lookup{命令查找}
    
    cmd_lookup --> cmd_help["cmdHelp()"]
    cmd_lookup --> cmd_who["cmdWho()"]
    cmd_lookup --> cmd_look["cmdLook()"]
    cmd_lookup --> cmd_say["cmdSay()"]
    cmd_lookup --> cmd_name["cmdName()"]
    cmd_lookup --> cmd_move["cmdMove()"]
    cmd_lookup --> cmd_kill["cmdKill()"]
    cmd_lookup --> cmd_attack["cmdAttack()"]
    cmd_lookup --> cmd_save["cmdSave()"]
    
    cmd_who --> getOnlinePlayers["MudServer::getOnlinePlayers()"]
    
    cmd_look --> getCurrentRoom["Session::getCurrentRoom()"]
    cmd_look --> getRoomData["Room::getExits/NPCs/Monsters()"]
    
    cmd_say --> broadcastToRoom["MudServer::broadcastToRoom()"]
    
    cmd_move --> getExit["Room::getExit()"]
    cmd_move --> setCurrentRoom["Session::setCurrentRoom()"]
    
    cmd_kill --> getMonster["Room::getMonster()"]
    cmd_kill --> startCombat["CombatManager::startCombat()"]
    
    cmd_attack --> playerAttack["CombatManager::playerAttack()"]
    
    cmd_save --> savePlayer["SaveManager::savePlayer()"]
```

---

## 4. 战斗系统调用关系

```mermaid
graph TB
    startCombat["CombatManager::startCombat()"] --> sendStart["Session::send(战斗开始)"]
    
    playerAttack["CombatManager::playerAttack()"] --> checkAlive{"Monster 存活?"}
    checkAlive -->|是 | calcDamage["计算伤害"]
    calcDamage --> monsterTakeDmg["Monster::takeDamage()"]
    monsterTakeDmg --> checkDead{"Monster 死亡?"}
    
    checkDead -->|否 | monsterAttack["CombatManager::monsterAttack()"]
    monsterAttack --> monsterAtk["Monster::attack()"]
    monsterAtk --> playerTakeDmg["玩家扣血"]
    
    checkDead -->|是 | victory["胜利"]
    victory --> addExp["Player::addExp()"]
    victory --> endCombat["CombatManager::endCombat()"]
    
    checkAlive -->|否 | endCombat
```

---

## 5. 客户端调用关系

```mermaid
graph LR
    main_client["main()"] --> MudClient_new["MudClient(host, port)"]
    MudClient_new --> MudClient_start["MudClient::start()"]
    
    MudClient_start --> connect["MudClient::connect()"]
    connect --> net_connect["network::connect_to_server()"]
    
    MudClient_start --> readLoop_client["readLoop 线程"]
    readLoop_client --> net_recv_client["network::recv_data()"]
    readLoop_client --> cout["std::cout 输出"]
    
    MudClient_start --> input_loop["主线程输入循环"]
    input_loop --> std_getline["std::getline()"]
    std_getline --> client_send["MudClient::send()"]
    client_send --> net_send_client["network::send_data()"]
```

---

## 6. 存档系统调用关系

```mermaid
graph TB
    cmd_save["CommandHandler::save()"] --> genFilename["SaveManager::generateFilename()"]
    genFilename --> savePlayer["SaveManager::savePlayer()"]
    savePlayer --> toJson["toJson()"]
    toJson --> file_write["std::ofstream 写入"]
    
    cmd_load["CommandHandler::load()"] --> loadPlayer["SaveManager::loadPlayer()"]
    loadPlayer --> file_read["std::ifstream 读取"]
    file_read --> fromJson["fromJson()"]
    fromJson --> player_fromData["Player::fromData()"]
    
    player_fromData --> setStats["设置玩家属性"]
    setStats --> setLevel["level, exp, hp, mp..."]
    setStats --> setInventory["inventory"]
```

---

## 7. 类依赖关系

```mermaid
classDiagram
    class MudServer {
        -World world_
        -sessions_
        +start()
        +stop()
        +addSession()
    }
    
    class Session {
        -Player player_
        -CommandHandler command_handler_
        -CombatManager combat_manager_
        +start()
        +readLoop()
        +writeLoop()
    }
    
    class CommandHandler {
        -commands_
        +handleCommand()
        +cmdLook()
        +cmdMove()
        +cmdKill()
    }
    
    class CombatManager {
        -Monster monster_
        -state_
        +startCombat()
        +playerAttack()
        +monsterAttack()
    }
    
    class Player {
        -name_, level_, hp_, mp_
        -inventory_
        +addExp()
        +takeDamage()
        +toData()
    }
    
    class World {
        -rooms_
        +getRoom()
        +move()
    }
    
    class Room {
        -exits_, npcs_, monsters_
        +getExit()
        +getMonster()
    }
    
    class SaveManager {
        +savePlayer()
        +loadPlayer()
        +toJson()
    }
    
    MudServer "1" *-- "*" Session
    Session "1" *-- "1" Player
    Session "1" *-- "1" CommandHandler
    Session "1" *-- "1" CombatManager
    Session "1" --> "1" Room
    CommandHandler --> World
    CommandHandler --> MudServer
    CombatManager --> Monster
    World "1" *-- "*" Room
    Room "1" *-- "*" Monster
```

---

## 8. 网络层调用关系

```mermaid
graph TB
    subgraph Server_Net["服务器网络"]
        net_init["network::init()"] --> WSAStartup["WSAStartup()"]
        net_create["network::create_tcp_socket()"] --> socket["socket()"]
        net_bind["network::bind_socket()"] --> bind["bind()"]
        net_listen["network::listen_socket()"] --> listen["listen()"]
        net_accept["network::accept_socket()"] --> accept["accept()"]
    end
    
    subgraph Client_Net["客户端网络"]
        net_connect["network::connect_to_server()"] --> getaddrinfo["getaddrinfo()"]
        getaddrinfo --> connect["connect()"]
    end
    
    net_recv["network::recv_data()"] --> recv["recv()"]
    net_send["network::send_data()"] --> send["send()"]
    net_close["network::close_socket()"] --> closesocket["closesocket()"]
```

---

## 9. 完整调用链路示例

### 玩家登录流程
```
main() 
  → MudServer::start() 
    → network::init/bind/listen() 
    → accept 循环 
      → Session::start() 
        → 发送欢迎消息
        → 启动 readLoop/writeLoop 线程
```

### 玩家移动流程
```
玩家输入 "north"
  → Session::readLoop() 
    → CommandHandler::handleCommand() 
      → cmdMove() 
        → Room::getExit(north) 
        → Session::setCurrentRoom() 
        → MudServer::broadcastToRoom()
```

### 战斗流程
```
玩家输入 "kill Goblin"
  → CommandHandler::cmdKill() 
    → Room::getMonster("Goblin") 
    → CombatManager::startCombat() 
      → Session::send(战斗开始)

玩家输入 "attack"
  → CommandHandler::cmdAttack() 
    → CombatManager::playerAttack() 
      → Monster::takeDamage() 
      → Monster::attack() (反击)
      → Session::send(战斗结果)
```
