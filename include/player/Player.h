#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <cstdint>

namespace mud {

class Player {
public:
    Player();
    explicit Player(const std::string& name);
    
    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }
    
    const std::string& getDescription() const { return description_; }
    void setDescription(const std::string& desc) { description_ = desc; }
    
    const std::string& getLocation() const { return location_; }
    void setLocation(const std::string& loc) { location_ = loc; }
    
    bool isLoggedIn() const { return logged_in_; }
    void setLoggedIn(bool logged_in) { logged_in_ = logged_in; }
    
    std::string getInfo() const;
    
private:
    std::string name_ = "Stranger";
    std::string description_ = "A mysterious adventurer";
    std::string location_ = "Starting Hall";
    bool logged_in_ = false;
};

} // namespace mud

#endif // PLAYER_H
