#include "save/SaveManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace mud {

SaveManager::SaveManager() {
    // 默认存档目录
    save_dir_ = "saves";
    
    // 创建存档目录
    std::filesystem::create_directories(save_dir_);
}

SaveManager::~SaveManager() {}

SaveManager& SaveManager::getInstance() {
    static SaveManager instance;
    return instance;
}

void SaveManager::setSaveDirectory(const std::string& dir) {
    save_dir_ = dir;
    std::filesystem::create_directories(save_dir_);
}

std::string SaveManager::generateFilename(const std::string& characterName) {
    // 将角色名转换为安全的文件名
    std::string filename = characterName;
    std::transform(filename.begin(), filename.end(), filename.begin(),
                   [](char c) { return std::isalnum(c) ? c : '_'; });
    return filename + ".sav";
}

bool SaveManager::savePlayer(const std::string& filename, const PlayerData& data) {
    std::string filepath = save_dir_ + "/" + filename;
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string json = toJson(data);
    file << json;
    file.close();
    
    return true;
}

bool SaveManager::loadPlayer(const std::string& filename, PlayerData& data) {
    std::string filepath = save_dir_ + "/" + filename;
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string json = buffer.str();
    file.close();
    
    if (json.empty()) {
        return false;
    }
    
    data = fromJson(json);
    return true;
}

bool SaveManager::hasSave(const std::string& filename) const {
    std::string filepath = save_dir_ + "/" + filename;
    return std::filesystem::exists(filepath);
}

bool SaveManager::deleteSave(const std::string& filename) {
    std::string filepath = save_dir_ + "/" + filename;
    if (std::filesystem::exists(filepath)) {
        return std::filesystem::remove(filepath);
    }
    return false;
}

std::vector<std::string> SaveManager::getSaveList() const {
    std::vector<std::string> saves;
    
    for (const auto& entry : std::filesystem::directory_iterator(save_dir_)) {
        if (entry.path().extension() == ".sav") {
            saves.push_back(entry.path().stem().string());
        }
    }
    
    return saves;
}

// ==================== JSON 序列化/反序列化 ====================

std::string SaveManager::toJson(const PlayerData& data) const {
    std::ostringstream oss;
    
    oss << "{\n";
    oss << "  \"name\": \"" << data.name << "\",\n";
    oss << "  \"description\": \"" << data.description << "\",\n";
    oss << "  \"location\": \"" << data.location << "\",\n";
    oss << "  \"level\": " << data.level << ",\n";
    oss << "  \"exp\": " << data.exp << ",\n";
    oss << "  \"max_hp\": " << data.max_hp << ",\n";
    oss << "  \"current_hp\": " << data.current_hp << ",\n";
    oss << "  \"max_mp\": " << data.max_mp << ",\n";
    oss << "  \"current_mp\": " << data.current_mp << ",\n";
    oss << "  \"attack\": " << data.attack << ",\n";
    oss << "  \"defense\": " << data.defense << ",\n";
    oss << "  \"gold\": " << data.gold << ",\n";
    oss << "  \"inventory\": [\n";
    
    for (size_t i = 0; i < data.inventory.size(); ++i) {
        const auto& item = data.inventory[i];
        oss << "    {\n";
        oss << "      \"id\": \"" << item.id << "\",\n";
        oss << "      \"name\": \"" << item.name << "\",\n";
        oss << "      \"description\": \"" << item.description << "\",\n";
        oss << "      \"value\": " << item.value << "\n";
        oss << "    }";
        if (i < data.inventory.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    
    oss << "  ]\n";
    oss << "}\n";
    
    return oss.str();
}

PlayerData SaveManager::fromJson(const std::string& json) const {
    PlayerData data;
    
    // 简单的 JSON 解析（针对我们自己的格式）
    auto extractString = [&json](const std::string& key) -> std::string {
        std::string search = "\"" + key + "\": \"";
        size_t pos = json.find(search);
        if (pos == std::string::npos) return "";
        
        pos += search.length();
        size_t end = json.find("\"", pos);
        if (end == std::string::npos) return "";
        
        return json.substr(pos, end - pos);
    };
    
    auto extractInt = [&json](const std::string& key) -> int {
        std::string search = "\"" + key + "\": ";
        size_t pos = json.find(search);
        if (pos == std::string::npos) return 0;
        
        pos += search.length();
        std::string numStr;
        while (pos < json.size() && (std::isdigit(json[pos]) || json[pos] == '-')) {
            numStr += json[pos++];
        }
        
        return numStr.empty() ? 0 : std::stoi(numStr);
    };
    
    data.name = extractString("name");
    data.description = extractString("description");
    data.location = extractString("location");
    data.level = extractInt("level");
    data.exp = extractInt("exp");
    data.max_hp = extractInt("max_hp");
    data.current_hp = extractInt("current_hp");
    data.max_mp = extractInt("max_mp");
    data.current_mp = extractInt("current_mp");
    data.attack = extractInt("attack");
    data.defense = extractInt("defense");
    data.gold = extractInt("gold");
    
    // 解析物品列表（简化版）
    // 查找 inventory 数组
    size_t invStart = json.find("\"inventory\": [");
    if (invStart != std::string::npos) {
        size_t invEnd = json.find("]", invStart);
        std::string invJson = json.substr(invStart, invEnd - invStart + 1);
        
        // 计算物品数量
        size_t pos = 0;
        while ((pos = invJson.find("\"name\":", pos)) != std::string::npos) {
            pos++;
            
            // 提取物品信息
            size_t idPos = invJson.find("\"id\":", pos - 1);
            size_t namePos = invJson.find("\"name\":", pos - 1);
            size_t descPos = invJson.find("\"description\":", pos - 1);
            size_t valuePos = invJson.find("\"value\":", pos - 1);
            
            if (idPos != std::string::npos && namePos != std::string::npos) {
                Item item;
                
                // 提取 id
                idPos += 6;
                size_t idEnd = invJson.find("\"", idPos);
                if (idEnd != std::string::npos) {
                    item.id = invJson.substr(idPos, idEnd - idPos);
                }
                
                // 提取 name
                namePos += 8;
                size_t nameEnd = invJson.find("\"", namePos);
                if (nameEnd != std::string::npos) {
                    item.name = invJson.substr(namePos, nameEnd - namePos);
                }
                
                // 提取 description
                if (descPos != std::string::npos) {
                    descPos += 15;
                    size_t descEnd = invJson.find("\"", descPos);
                    if (descEnd != std::string::npos) {
                        item.description = invJson.substr(descPos, descEnd - descPos);
                    }
                }
                
                // 提取 value
                if (valuePos != std::string::npos) {
                    valuePos += 9;
                    std::string valueStr;
                    while (valuePos < invJson.size() && std::isdigit(invJson[valuePos])) {
                        valueStr += invJson[valuePos++];
                    }
                    if (!valueStr.empty()) {
                        item.value = std::stoi(valueStr);
                    }
                }
                
                data.inventory.push_back(item);
            }
        }
    }
    
    return data;
}

} // namespace mud
