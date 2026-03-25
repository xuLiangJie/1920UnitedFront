cmake_minimum_required(VERSION 3.15)
project(MudGame VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 链接线程库
find_package(Threads REQUIRED)

# 服务器可执行文件
add_executable(mud_server
    src/server/MudServer.cpp
    src/server/Session.cpp
    src/command/CommandHandler.cpp
    src/player/Player.cpp
    src/world/Room.cpp
    src/combat/Monster.cpp
    src/combat/Combat.cpp
    src/network/Network.cpp
    src/main_server.cpp
)
target_include_directories(mud_server PRIVATE include)
target_link_libraries(mud_server PRIVATE Threads::Threads)

# 客户端可执行文件
add_executable(mud_client
    src/client/MudClient.cpp
    src/network/Network.cpp
    src/main_client.cpp
)
target_include_directories(mud_client PRIVATE include)
target_link_libraries(mud_client PRIVATE Threads::Threads)

# Windows 特定设置
if(WIN32)
    target_compile_definitions(mud_server PRIVATE _WIN32_WINNT=0x0601)
    target_compile_definitions(mud_client PRIVATE _WIN32_WINNT=0x0601)
    target_link_libraries(mud_server PRIVATE ws2_32)
    target_link_libraries(mud_client PRIVATE ws2_32)
endif()

# 设置编译选项
if(MSVC)
    target_compile_options(mud_server PRIVATE /W4)
    target_compile_options(mud_client PRIVATE /W4)
else()
    target_compile_options(mud_server PRIVATE -Wall -Wextra)
    target_compile_options(mud_client PRIVATE -Wall -Wextra)
endif()
