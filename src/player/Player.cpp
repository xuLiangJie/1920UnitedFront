#include "player/Player.h"
#include <sstream>

namespace mud {

Player::Player() = default;

Player::Player(const std::string& name) : name_(name) {}

std::string Player::getInfo() const {
    std::ostringstream oss;
    oss << name_ << " - " << description_;
    return oss.str();
}

} // namespace mud
