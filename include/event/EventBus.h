#ifndef EVENT_BUS_H
#define EVENT_BUS_H

#include "event/Events.h"
#include <functional>
#include <vector>
#include <memory>
#include <mutex>
#include <unordered_map>

namespace mud {

// 事件处理器类型
using EventHandler = std::function<void(const Event&)>;

// 事件总线 - 观察者模式实现
class EventBus {
public:
    static EventBus& getInstance();
    
    // 订阅事件
    void subscribe(EventType type, EventHandler handler);
    
    // 取消订阅
    void unsubscribe(EventType type);
    
    // 发布事件
    void publish(const Event& event);
    
    // 清空所有订阅
    void clear();
    
private:
    EventBus() = default;
    ~EventBus() = default;
    
    std::unordered_map<EventType, std::vector<EventHandler>> handlers_;
    std::mutex mutex_;
};

} // namespace mud

#endif // EVENT_BUS_H
