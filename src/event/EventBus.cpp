#include "event/EventBus.h"
#include <algorithm>
#include <iostream>

namespace mud {

EventBus& EventBus::getInstance() {
    static EventBus instance;
    return instance;
}

void EventBus::subscribe(EventType type, EventHandler handler) {
    std::lock_guard<std::mutex> lock(mutex_);
    handlers_[type].push_back(std::move(handler));
}

void EventBus::unsubscribe(EventType type) {
    std::lock_guard<std::mutex> lock(mutex_);
    handlers_.erase(type);
}

void EventBus::publish(const Event& event) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = handlers_.find(event.getType());
    if (it != handlers_.end()) {
        for (const auto& handler : it->second) {
            try {
                handler(event);
            } catch (const std::exception& e) {
                // 记录错误但不中断其他处理器
                std::cerr << "Event handler error: " << e.what() << std::endl;
            }
        }
    }
}

void EventBus::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    handlers_.clear();
}

} // namespace mud
