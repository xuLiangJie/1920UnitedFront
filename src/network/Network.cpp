#include "network/Network.h"
#include <iostream>
#include <cstring>
#include <cerrno>

#ifdef _WIN32
static bool g_winsock_initialized = false;
#endif

namespace mud {
namespace network {

bool init() {
#ifdef _WIN32
    if (!g_winsock_initialized) {
        WSADATA wsa_data;
        int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
        if (result != 0) {
            std::cerr << "WSAStartup 失败：" << result << std::endl;
            return false;
        }
        g_winsock_initialized = true;
    }
#endif
    return true;
}

void cleanup() {
#ifdef _WIN32
    if (g_winsock_initialized) {
        WSACleanup();
        g_winsock_initialized = false;
    }
#endif
}

int create_tcp_socket() {
    int fd = static_cast<int>(socket(AF_INET, SOCK_STREAM, IPPROTO_TCP));
    if (fd == static_cast<int>(SOCKET_INVALID)) {
        std::cerr << "创建套接字失败：" << get_last_error() << std::endl;
        return -1;
    }
    
    // 设置地址复用
    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, 
               reinterpret_cast<const char*>(&opt), sizeof(opt));
    
    return fd;
}

bool set_nonblocking(int fd) {
#ifdef _WIN32
    u_long mode = 1;
    return ioctlsocket(fd, FIONBIO, &mode) == 0;
#else
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return false;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK) != -1;
#endif
}

bool bind_socket(int fd, short port) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    
    int result = bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    if (result == -1) {
        std::cerr << "绑定套接字失败：" << get_last_error() << std::endl;
        return false;
    }
    return true;
}

bool listen_socket(int fd, int backlog) {
    int result = listen(fd, backlog);
    if (result == -1) {
        std::cerr << "监听套接字失败：" << get_last_error() << std::endl;
        return false;
    }
    return true;
}

int accept_socket(int server_fd) {
    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    
    int client_fd = static_cast<int>(accept(server_fd, 
                           reinterpret_cast<sockaddr*>(&client_addr), 
                           &client_len));
    
    if (client_fd == static_cast<int>(SOCKET_INVALID)) {
        return -1;
    }
    
    // 获取客户端地址
    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, ip_str, sizeof(ip_str));
    std::cout << "新连接：" << ip_str << ":" << ntohs(client_addr.sin_port) << std::endl;
    
    return client_fd;
}

int connect_to_server(const std::string& host, short port) {
    int fd = create_tcp_socket();
    if (fd == -1) {
        return -1;
    }
    
    // 解析主机名
    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    
    addrinfo* result = nullptr;
    std::string port_str = std::to_string(port);
    
    int ret = getaddrinfo(host.c_str(), port_str.c_str(), &hints, &result);
    if (ret != 0) {
        std::cerr << "解析主机名失败：" << host << std::endl;
        close_socket(fd);
        return -1;
    }
    
    // 尝试连接
    for (addrinfo* rp = result; rp != nullptr; rp = rp->ai_next) {
        if (::connect(fd, rp->ai_addr, static_cast<socklen_t>(rp->ai_addrlen)) == 0) {
            freeaddrinfo(result);
            std::cout << "已连接到服务器：" << host << ":" << port << std::endl;
            return fd;
        }
    }
    
    freeaddrinfo(result);
    std::cerr << "连接服务器失败：" << host << ":" << port << std::endl;
    close_socket(fd);
    return -1;
}

ssize_t send_data(int fd, const void* data, size_t size) {
#ifdef _WIN32
    return send(fd, static_cast<const char*>(data), static_cast<int>(size), 0);
#else
    return ::send(fd, data, size, 0);
#endif
}

ssize_t recv_data(int fd, void* buffer, size_t size) {
#ifdef _WIN32
    return recv(fd, static_cast<char*>(buffer), static_cast<int>(size), 0);
#else
    return ::recv(fd, buffer, size, 0);
#endif
}

void close_socket(int fd) {
    if (fd != -1 && fd != static_cast<int>(SOCKET_INVALID)) {
        SOCKET_CLOSE(fd);
    }
}

std::string get_last_error() {
#ifdef _WIN32
    int error = WSAGetLastError();
    char buffer[256];
    strerror_s(buffer, sizeof(buffer), error);
    return std::string(buffer);
#else
    return std::string(strerror(errno));
#endif
}

} // namespace network
} // namespace mud
