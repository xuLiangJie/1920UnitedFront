#include "command/CommandHandler.h"
#include "server/Session.h"
#include "player/Player.h"
#include "server/MudServer.h"
#include "world/Room.h"
#include "combat/Monster.h"
#include "combat/Combat.h"
#include "save/SaveManager.h"
#include <sstream>
#include <algorithm>
#include <cctype>

namespace mud {

CommandHandler::CommandHandler() {
    commands_["help"] = [this](Session* s, const std::vector<std::string>& a) { return cmdHelp(s, a); };
    commands_["who"] = [this](Session* s, const std::vector<std::string>& a) { return cmdWho(s, a); };
    commands_["look"] = [this](Session* s, const std::vector<std::string>& a) { return cmdLook(s, a); };
    commands_["say"] = [this](Session* s, const std::vector<std::string>& a) { return cmdSay(s, a); };
    commands_["name"] = [this](Session* s, const std::vector<std::string>& a) { return cmdName(s, a); };
    commands_["score"] = [this](Session* s, const std::vector<std::string>& a) { return cmdScore(s, a); };
    commands_["quit"] = [this](Session* s, const std::vector<std::string>& a) { return cmdQuit(s, a); };
    commands_["n"] = [this](Session* s, const std::vector<std::string>& a) { return cmdMove(s, a, Direction::North); };
    commands_["north"] = [this](Session* s, const std::vector<std::string>& a) { return cmdMove(s, a, Direction::North); };
    commands_["s"] = [this](Session* s, const std::vector<std::string>& a) { return cmdMove(s, a, Direction::South); };
    commands_["south"] = [this](Session* s, const std::vector<std::string>& a) { return cmdMove(s, a, Direction::South); };
    commands_["e"] = [this](Session* s, const std::vector<std::string>& a) { return cmdMove(s, a, Direction::East); };
    commands_["east"] = [this](Session* s, const std::vector<std::string>& a) { return cmdMove(s, a, Direction::East); };
    commands_["w"] = [this](Session* s, const std::vector<std::string>& a) { return cmdMove(s, a, Direction::West); };
    commands_["west"] = [this](Session* s, const std::vector<std::string>& a) { return cmdMove(s, a, Direction::West); };
    commands_["i"] = [this](Session* s, const std::vector<std::string>& a) { return cmdInventory(s, a); };
    commands_["inventory"] = [this](Session* s, const std::vector<std::string>& a) { return cmdInventory(s, a); };
    commands_["kill"] = [this](Session* s, const std::vector<std::string>& a) { return cmdKill(s, a); };
    commands_["attack"] = [this](Session* s, const std::vector<std::string>& a) { return cmdAttack(s, a); };
    commands_["flee"] = [this](Session* s, const std::vector<std::string>& a) { return cmdFlee(s, a); };
    commands_["save"] = [this](Session* s, const std::vector<std::string>& a) { return cmdSave(s, a); };
    commands_["load"] = [this](Session* s, const std::vector<std::string>& a) { return cmdLoad(s, a); };
    commands_["delete"] = [this](Session* s, const std::vector<std::string>& a) { return cmdDelete(s, a); };
    commands_["saves"] = [this](Session* s, const std::vector<std::string>& a) { return cmdSaves(s, a); };
}

std::string CommandHandler::handleCommand(Session* session, const std::string& input) {
    if (input.empty()) {
        return "";
    }
    
    auto args = parseInput(input);
    if (args.empty()) {
        return "";
    }
    
    std::string cmd = args[0];
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    
    auto it = commands_.find(cmd);
    if (it != commands_.end()) {
        return it->second(session, args);
    } else {
        return "Unknown command. Type 'help' to see available commands.\r\n";
    }
}

std::vector<std::string> CommandHandler::parseInput(const std::string& input) {
    std::vector<std::string> args;
    std::istringstream iss(input);
    std::string arg;
    
    while (iss >> arg) {
        args.push_back(arg);
    }
    
    return args;
}

std::string CommandHandler::cmdHelp(Session*, const std::vector<std::string>&) {
    std::ostringstream oss;
    oss << "\r\n====== Western Fantasy MUD - Command Help ======\r\n\r\n";
    oss << "[Basic Commands]\r\n";
    oss << "  help          - Show this help message\r\n";
    oss << "  who           - List online players\r\n";
    oss << "  look          - Look around (see room, NPCs, monsters)\r\n";
    oss << "  score         - Show your status\r\n";
    oss << "  name <name>   - Set your character name\r\n";
    oss << "  say <text>    - Say something to people in same room\r\n";
    oss << "\r\n";
    oss << "[Movement Commands]\r\n";
    oss << "  north (n)     - Move north\r\n";
    oss << "  south (s)     - Move south\r\n";
    oss << "  east (e)      - Move east\r\n";
    oss << "  west (w)      - Move west\r\n";
    oss << "\r\n";
    oss << "[Combat Commands]\r\n";
    oss << "  kill <monster>- Attack a monster\r\n";
    oss << "  attack        - Continue attacking current enemy\r\n";
    oss << "  flee          - Try to escape from combat\r\n";
    oss << "\r\n";
    oss << "[Save System]\r\n";
    oss << "  save          - Save your character progress\r\n";
    oss << "  load          - Load a saved character\r\n";
    oss << "  saves         - List all saved characters\r\n";
    oss << "  delete <name> - Delete a saved character\r\n";
    oss << "\r\n";
    oss << "[Other Commands]\r\n";
    oss << "  inventory (i) - Check your inventory\r\n";
    oss << "  quit          - Quit the game (auto-saves)\r\n";
    oss << "\r\n";
    oss << "====================================================\r\n";
    return oss.str();
}

std::string CommandHandler::cmdWho(Session* session, const std::vector<std::string>&) {
    std::ostringstream oss;
    oss << "\r\n====== Online Players ======\r\n";
    
    auto& server = session->getServer();
    auto players = server.getOnlinePlayers();
    
    if (players.empty()) {
        oss << "No players online.\r\n";
    } else {
        for (const auto& player : players) {
            oss << "  - " << player << "\r\n";
        }
    }
    oss << "==========================\r\n";
    return oss.str();
}

std::string CommandHandler::cmdLook(Session* session, const std::vector<std::string>&) {
    const Room* room = session->getCurrentRoom();
    if (!room) {
        return "\r\nYou are in nowhere. Something is wrong...\r\n";
    }
    
    std::ostringstream oss;
    oss << "\r\n====== " << room->getName() << " ======\r\n\r\n";
    oss << room->getDescription() << "\r\n\r\n";
    
    auto exits = room->getExits();
    if (!exits.empty()) {
        oss << "Visible exits: ";
        bool first = true;
        for (const auto& [dir, roomId] : exits) {
            if (!first) oss << ", ";
            oss << Room::directionToString(dir);
            first = false;
        }
        oss << "\r\n";
    }
    
    const auto& npcs = room->getNPCs();
    if (!npcs.empty()) {
        oss << "\r\nYou see here:\r\n";
        for (const auto& npc : npcs) {
            oss << "  - " << npc.name << ": " << npc.description << "\r\n";
        }
    }
    
    const auto& monsters = room->getMonsters();
    if (!monsters.empty()) {
        oss << "\r\n*** DANGEROUS CREATURES ***\r\n";
        for (const auto& monster : monsters) {
            if (monster.isAlive()) {
                oss << "  - " << monster.getName() << ": " << monster.getDescription() << "\r\n";
                oss << "    " << monster.getHealthStatus() << "\r\n";
            }
        }
        oss << "\r\nType 'kill <monster>' to attack!\r\n";
    }
    
    auto& server = session->getServer();
    auto playersInRoom = server.getPlayersInRoom(room->getId(), session->getId());
    if (!playersInRoom.empty()) {
        oss << "\r\nOther adventurers here:\r\n";
        for (const auto& player : playersInRoom) {
            oss << "  - " << player << "\r\n";
        }
    }
    
    oss << "\r\n";
    return oss.str();
}

std::string CommandHandler::cmdSay(Session* session, const std::vector<std::string>& args) {
    if (args.size() < 2) {
        return "Usage: say <text>\r\n";
    }
    
    auto& player = session->getPlayer();
    std::ostringstream oss;
    
    std::string message;
    for (size_t i = 1; i < args.size(); ++i) {
        if (i > 1) message += " ";
        message += args[i];
    }
    
    oss << "\r\nYou say: \"" << message << "\"\r\n";
    
    const Room* room = session->getCurrentRoom();
    if (room) {
        std::string broadcast_msg = player.getName() + " says: \"" + message + "\"\r\n";
        session->getServer().broadcastToRoom(room->getId(), broadcast_msg, session->getId());
    }
    
    return oss.str();
}

std::string CommandHandler::cmdName(Session* session, const std::vector<std::string>& args) {
    if (args.size() < 2) {
        return "Usage: name <character_name>\r\n";
    }
    
    auto& player = session->getPlayer();
    std::string new_name = args[1];
    
    if (new_name.length() < 2 || new_name.length() > 20) {
        return "Name must be between 2-20 characters.\r\n";
    }
    
    std::string old_name = player.getName();
    player.setName(new_name);
    player.setLoggedIn(true);
    
    std::ostringstream oss;
    oss << "\r\nYour name has been changed from \"" << old_name << "\" to \"" << new_name << "\".\r\n";
    
    const Room* room = session->getCurrentRoom();
    if (room) {
        session->getServer().broadcastToRoom(room->getId(), 
            player.getName() + " has entered the game.\r\n", session->getId());
    }
    
    return oss.str();
}

std::string CommandHandler::cmdScore(Session* session, const std::vector<std::string>&) {
    auto& player = session->getPlayer();
    const Room* room = session->getCurrentRoom();
    
    std::ostringstream oss;
    oss << "\r\n====== " << player.getName() << "'s Status ======\r\n";
    oss << "  Level: " << player.getLevel() << "\r\n";
    oss << "  Experience: " << player.getExp() << "\r\n";
    oss << "  Location: " << (room ? room->getName() : "Unknown") << "\r\n";
    oss << "  HP: " << player.getCurrentHP() << "/" << player.getMaxHP() << "\r\n";
    oss << "  MP: " << player.getCurrentMP() << "/" << player.getMaxMP() << "\r\n";
    oss << "  Attack: " << player.getAttack() << "\r\n";
    oss << "  Defense: " << player.getDefense() << "\r\n";
    oss << "  Gold: " << player.getGold() << "\r\n";
    oss << "  Items: " << player.getInventory().size() << "\r\n";
    oss << "============================================\r\n";
    
    return oss.str();
}

std::string CommandHandler::cmdQuit(Session* session, const std::vector<std::string>&) {
    auto& player = session->getPlayer();
    const Room* room = session->getCurrentRoom();
    
    // 自动保存
    std::string filename = SaveManager::generateFilename(player.getName());
    if (SaveManager::getInstance().savePlayer(filename, player.toData())) {
        std::ostringstream oss;
        oss << "\r\n[Auto-Save] Game saved as '" << filename << "'\r\n";
        session->send(oss.str());
    }
    
    std::ostringstream oss;
    oss << "\r\nThank you for playing, " << player.getName() << "! See you again.\r\n";
    oss << "Quitting game...\r\n";
    
    if (room) {
        session->getServer().broadcastToRoom(room->getId(), 
            player.getName() + " has left the game.\r\n", session->getId());
    }
    
    session->stop();
    
    return oss.str();
}

std::string CommandHandler::cmdMove(Session* session, const std::vector<std::string>&, Direction dir) {
    if (session->isInCombat()) {
        return "\r\nYou cannot flee while in combat! Use 'flee' to try escaping!\r\n";
    }
    
    const Room* currentRoom = session->getCurrentRoom();
    if (!currentRoom) {
        return "\r\nYou are lost in nowhere...\r\n";
    }
    
    std::string nextRoomId = currentRoom->getExit(dir);
    if (nextRoomId.empty()) {
        return "\r\nYou cannot go that way.\r\n";
    }
    
    std::string leave_msg = session->getPlayer().getName() + " leaves " + 
                           Room::directionToString(dir) + ".\r\n";
    session->getServer().broadcastToRoom(currentRoom->getId(), leave_msg, session->getId());
    
    session->setCurrentRoom(nextRoomId);
    const Room* newRoom = session->getCurrentRoom();
    
    if (!newRoom) {
        return "\r\nSomething went wrong. You are in nowhere.\r\n";
    }
    
    std::ostringstream oss;
    oss << "\r\nYou walk " << Room::directionToString(dir) << " and arrive at " 
        << newRoom->getName() << ".\r\n";
    
    std::string arrive_msg = session->getPlayer().getName() + " arrives from " + 
                            oppositeDirectionToString(dir) + ".\r\n";
    session->getServer().broadcastToRoom(newRoom->getId(), arrive_msg, session->getId());
    
    oss << "\r\n====== " << newRoom->getName() << " ======\r\n\r\n";
    oss << newRoom->getDescription() << "\r\n\r\n";
    
    auto exits = newRoom->getExits();
    if (!exits.empty()) {
        oss << "Visible exits: ";
        bool first = true;
        for (const auto& [d, roomId] : exits) {
            if (!first) oss << ", ";
            oss << Room::directionToString(d);
            first = false;
        }
        oss << "\r\n";
    }
    
    const auto& npcs = newRoom->getNPCs();
    if (!npcs.empty()) {
        oss << "\r\nYou see here:\r\n";
        for (const auto& npc : npcs) {
            oss << "  - " << npc.name << ": " << npc.description << "\r\n";
        }
    }
    
    const auto& monsters = newRoom->getMonsters();
    if (!monsters.empty()) {
        oss << "\r\n*** DANGEROUS CREATURES ***\r\n";
        for (const auto& monster : monsters) {
            if (monster.isAlive()) {
                oss << "  - " << monster.getName() << ": " << monster.getDescription() << "\r\n";
            }
        }
        oss << "\r\nType 'kill <monster>' to attack!\r\n";
    }
    
    auto playersInRoom = session->getServer().getPlayersInRoom(newRoom->getId(), session->getId());
    if (!playersInRoom.empty()) {
        oss << "\r\nOther adventurers here:\r\n";
        for (const auto& p : playersInRoom) {
            oss << "  - " << p << "\r\n";
        }
    }
    
    oss << "\r\n";
    return oss.str();
}

std::string CommandHandler::oppositeDirectionToString(Direction dir) {
    switch (dir) {
        case Direction::North: return "south";
        case Direction::South: return "north";
        case Direction::East:  return "west";
        case Direction::West:  return "east";
        case Direction::Up:    return "below";
        case Direction::Down:  return "above";
        case Direction::Enter: return "outside";
        case Direction::Exit:  return "inside";
        default: return "somewhere";
    }
}

std::string CommandHandler::cmdInventory(Session* session, const std::vector<std::string>&) {
    auto& player = session->getPlayer();
    const auto& inventory = player.getInventory();
    
    std::ostringstream oss;
    oss << "\r\n====== Inventory ======\r\n";
    
    if (inventory.empty()) {
        oss << "You are not carrying anything.\r\n";
    } else {
        oss << "You are carrying:\r\n";
        for (const auto& item : inventory) {
            oss << "  - " << item.name << ": " << item.description << "\r\n";
        }
    }
    oss << "=======================\r\n";
    
    return oss.str();
}

// ==================== 战斗命令 ====================

std::string CommandHandler::cmdKill(Session* session, const std::vector<std::string>& args) {
    if (args.size() < 2) {
        return "Usage: kill <monster_name>\r\nType 'look' to see what monsters are here.\r\n";
    }
    
    if (session->isInCombat()) {
        return "\r\nYou are already in combat! Use 'attack' to continue fighting,\r\n"
               "or 'flee' to try to escape!\r\n";
    }
    
    Room* room = session->getCurrentRoom();
    if (!room) {
        return "\r\nYou are in nowhere...\r\n";
    }
    
    std::string monsterName = args[1];
    monsterName[0] = std::toupper(monsterName[0]);
    
    Monster* monster = room->getMonster(monsterName);
    if (!monster || !monster->isAlive()) {
        return "\r\nThere is no such monster here.\r\n";
    }
    
    session->getCombatManager().startCombat(session, *monster);
    room->removeMonster(monsterName);
    
    return "";
}

std::string CommandHandler::cmdAttack(Session* session, const std::vector<std::string>&) {
    if (!session->isInCombat()) {
        return "\r\nYou are not in combat. Use 'kill <monster>' to start fighting!\r\n";
    }
    
    CombatResult result = session->getCombatManager().playerAttack(session);
    
    // 如果战斗胜利，添加经验值
    if (result.playerWon && result.expGained > 0) {
        auto& player = session->getPlayer();
        player.addExp(result.expGained);
        
        std::ostringstream oss;
        oss << result.message;
        oss << "\r\n[Level Up!] You are now level " << player.getLevel() << "!\r\n";
        oss << "HP and MP restored, Attack and Defense increased!\r\n";
        return oss.str();
    }
    
    return result.message;
}

std::string CommandHandler::cmdFlee(Session* session, const std::vector<std::string>&) {
    if (!session->isInCombat()) {
        return "\r\nYou are not in combat.\r\n";
    }
    
    bool success = (rand() % 100) < 50;
    
    std::ostringstream oss;
    if (success) {
        oss << "\r\nYou manage to escape from the fight!\r\n";
        
        session->setCurrentRoom("hall");
        session->getCombatManager().endCombat(session, false);
        
        const Room* newRoom = session->getCurrentRoom();
        oss << "You arrive at " << newRoom->getName() << ", panting heavily.\r\n";
    } else {
        oss << "\r\nYou try to flee, but the monster blocks your path!\r\n";
        
        CombatResult result = session->getCombatManager().monsterAttack(session);
        oss << result.message;
    }
    
    return oss.str();
}

// ==================== 存档命令 ====================

std::string CommandHandler::cmdSave(Session* session, const std::vector<std::string>&) {
    auto& player = session->getPlayer();
    
    if (player.getName() == "Stranger") {
        return "\r\nYou need to set a name first! Use 'name <yourname>'\r\n";
    }
    
    std::string filename = SaveManager::generateFilename(player.getName());
    
    if (SaveManager::getInstance().savePlayer(filename, player.toData())) {
        std::ostringstream oss;
        oss << "\r\n[Save] Game saved successfully as '" << filename << "'\r\n";
        oss << "Your progress has been preserved!\r\n";
        return oss.str();
    } else {
        return "\r\n[Error] Failed to save game. Please try again.\r\n";
    }
}

std::string CommandHandler::cmdLoad(Session* session, const std::vector<std::string>& args) {
    std::string characterName;
    
    if (args.size() >= 2) {
        characterName = args[1];
    } else {
        // 使用当前角色名
        characterName = session->getPlayer().getName();
        if (characterName == "Stranger") {
            return "\r\nUsage: load <character_name>\r\n"
                   "Or set your name first with 'name <yourname>'\r\n";
        }
    }
    
    std::string filename = SaveManager::generateFilename(characterName);
    
    if (!SaveManager::getInstance().hasSave(filename)) {
        std::ostringstream oss;
        oss << "\r\n[Error] No save file found for '" << characterName << "'\r\n";
        oss << "Use 'saves' to see all saved characters.\r\n";
        return oss.str();
    }
    
    PlayerData data;
    if (!SaveManager::getInstance().loadPlayer(filename, data)) {
        return "\r\n[Error] Failed to load save file. It may be corrupted.\r\n";
    }
    
    // 应用加载的数据
    auto& player = session->getPlayer();
    player.fromData(data);
    session->setCurrentRoom(data.location);
    
    std::ostringstream oss;
    oss << "\r\n[Load] Welcome back, " << player.getName() << "!\r\n";
    oss << "Loaded character:\r\n";
    oss << "  Level: " << player.getLevel() << "\r\n";
    oss << "  HP: " << player.getCurrentHP() << "/" << player.getMaxHP() << "\r\n";
    oss << "  MP: " << player.getCurrentMP() << "/" << player.getMaxMP() << "\r\n";
    oss << "  Gold: " << player.getGold() << "\r\n";
    oss << "  Items: " << player.getInventory().size() << "\r\n";
    oss << "\r\nYou are now at " << session->getCurrentRoom()->getName() << ".\r\n";
    
    return oss.str();
}

std::string CommandHandler::cmdDelete(Session* session, const std::vector<std::string>& args) {
    if (args.size() < 2) {
        return "\r\nUsage: delete <character_name>\r\n"
               "Warning: This will permanently delete the save file!\r\n";
    }
    
    std::string characterName = args[1];
    std::string filename = SaveManager::generateFilename(characterName);
    
    if (!SaveManager::getInstance().hasSave(filename)) {
        std::ostringstream oss;
        oss << "\r\n[Error] No save file found for '" << characterName << "'\r\n";
        return oss.str();
    }
    
    if (SaveManager::getInstance().deleteSave(filename)) {
        std::ostringstream oss;
        oss << "\r\n[Delete] Save file '" << filename << "' has been deleted.\r\n";
        return oss.str();
    } else {
        return "\r\n[Error] Failed to delete save file.\r\n";
    }
}

std::string CommandHandler::cmdSaves(Session*, const std::vector<std::string>&) {
    auto saves = SaveManager::getInstance().getSaveList();
    
    std::ostringstream oss;
    oss << "\r\n====== Saved Characters ======\r\n";
    
    if (saves.empty()) {
        oss << "No saved characters found.\r\n";
    } else {
        for (const auto& save : saves) {
            oss << "  - " << save << "\r\n";
        }
    }
    oss << "==============================\r\n";
    oss << "Use 'load <name>' to load a character.\r\n";
    oss << "Use 'delete <name>' to delete a save.\r\n";
    
    return oss.str();
}

} // namespace mud
