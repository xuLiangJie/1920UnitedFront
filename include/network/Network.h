#ifndef NETWORK_H
#define NETWORK_H

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <netdb.h>
    #include <fcntl.h>
    #include <errno.h>
#endif

#include <string>

#ifdef _WIN32
    #define SOCKET_INVALID INVALID_SOCKET
    #define SOCKET_CLOSE closesocket
    #define GET_SOCKET_ERROR() WSAGetLastError()
#else
    #define SOCKET_INVALID (-1)
    #define SOCKET_CLOSE close
    #define GET_SOCKET_ERROR() errno
#endif

namespace mud {
namespace network {

// 初始化网络库
bool init();

// 清理网络库
void cleanup();

// 创建 TCP 套接字
int create_tcp_socket();

// 设置套接字为非阻塞
bool set_nonblocking(int fd);

// 绑定套接字
bool bind_socket(int fd, short port);

// 监听套接字
bool listen_socket(int fd, int backlog = 10);

// 接受连接
int accept_socket(int server_fd);

// 连接到服务器
int connect_to_server(const std::string& host, short port);

// 发送数据
ssize_t send_data(int fd, const void* data, size_t size);

// 接收数据
ssize_t recv_data(int fd, void* buffer, size_t size);

// 关闭套接字
void close_socket(int fd);

// 获取错误信息
std::string get_last_error();

} // namespace network
} // namespace mud

#endif // NETWORK_H
