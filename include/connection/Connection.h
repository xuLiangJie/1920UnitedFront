#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>
#include <queue>
#include <mutex>
#include <atomic>
#include <functional>

namespace mud {

// 网络连接类 - 从Session中分离出来的网络I/O职责
class Connection {
public:
    using MessageCallback = std::function<void(const std::string&)>;
    using CloseCallback = std::function<void()>;
    
    Connection(int socket_fd);
    ~Connection();
    
    // 启动连接(启动读写线程)
    void start();
    
    // 停止连接
    void stop();
    
    // 发送消息(线程安全)
    void send(const std::string& message);
    
    // 获取socket fd
    int getSocketFd() const { return socket_fd_; }
    
    // 连接状态
    bool isRunning() const { return !stopped_; }
    
    // 设置回调
    void onMessage(MessageCallback callback) { on_message_ = std::move(callback); }
    void onClose(CloseCallback callback) { on_close_ = std::move(callback); }
    
private:
    // 读取循环
    void readLoop();
    
    // 写入循环
    void writeLoop();
    
    int socket_fd_;
    std::queue<std::string> write_queue_;
    std::mutex write_mutex_;
    std::atomic<bool> stopped_{false};
    
    MessageCallback on_message_;
    CloseCallback on_close_;
};

} // namespace mud

#endif // CONNECTION_H
