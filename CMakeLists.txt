cmake_minimum_required(VERSION 3.15)
project(MudGame VERSION 1.0 LANGUAGES CXX)

# 启用compile_commands.json生成
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 选项
option(MUD_ENABLE_TESTS "Enable unit tests" OFF)
option(MUD_ENABLE_STRICT_WARNINGS "Enable strict compiler warnings" OFF)

# 链接线程库
find_package(Threads REQUIRED)

# 服务器可执行文件
add_executable(mud_server
    src/server/MudServer.cpp
    src/server/Session.cpp
    src/command/CommandHandler.cpp
    src/command/CommandRegistry.cpp
    src/command/HelpCommand.cpp
    src/command/LookCommand.cpp
    src/command/WhoCommand.cpp
    src/command/ScoreCommand.cpp
    src/command/NameCommand.cpp
    src/command/SayCommand.cpp
    src/command/QuitCommand.cpp
    src/command/InventoryCommand.cpp
    src/command/MoveCommand.cpp
    src/command/KillCommand.cpp
    src/command/AttackCommand.cpp
    src/command/FleeCommand.cpp
    src/command/SaveCommand.cpp
    src/command/LoadCommand.cpp
    src/command/DeleteCommand.cpp
    src/command/SavesCommand.cpp
    src/player/Player.cpp
    src/world/Room.cpp
    src/combat/Monster.cpp
    src/combat/Combat.cpp
    src/save/SaveManager.cpp
    src/network/Network.cpp
    src/event/EventBus.cpp
    src/connection/Connection.cpp
    src/strategy/DamageStrategy.cpp
    src/inventory/Inventory.cpp
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

# 严格警告选项
if(MUD_ENABLE_STRICT_WARNINGS)
    if(MSVC)
        target_compile_options(mud_server PRIVATE /WX)
        target_compile_options(mud_client PRIVATE /WX)
    else()
        target_compile_options(mud_server PRIVATE -Werror)
        target_compile_options(mud_client PRIVATE -Werror)
    endif()
endif()

# 单元测试配置
if(MUD_ENABLE_TESTS)
    enable_testing()
    
    # 使用FetchContent获取Google Test
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
        GIT_SHALLOW TRUE
    )
    # 防止GoogleTest的安装
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    # 测试可执行文件
    add_executable(mud_tests
        tests/test_player.cpp
        tests/test_monster.cpp
        tests/test_room.cpp
        tests/test_inventory.cpp
        tests/test_commands.cpp
        tests/test_save_manager.cpp
        tests/test_damage_strategy.cpp
        tests/test_event_bus.cpp
        tests/test_command_registry_extended.cpp
        tests/test_combat_manager.cpp
        # 需要链接被测试的源文件
        src/player/Player.cpp
        src/combat/Monster.cpp
        src/combat/Combat.cpp
        src/world/Room.cpp
        src/inventory/Inventory.cpp
        src/save/SaveManager.cpp
        src/strategy/DamageStrategy.cpp
        src/server/Session.cpp
        src/server/MudServer.cpp
        src/connection/Connection.cpp
        src/event/EventBus.cpp
        src/network/Network.cpp
        src/command/CommandRegistry.cpp
        src/command/CommandHandler.cpp
        src/command/HelpCommand.cpp
        src/command/LookCommand.cpp
        src/command/WhoCommand.cpp
        src/command/ScoreCommand.cpp
        src/command/NameCommand.cpp
        src/command/SayCommand.cpp
        src/command/QuitCommand.cpp
        src/command/InventoryCommand.cpp
        src/command/MoveCommand.cpp
        src/command/KillCommand.cpp
        src/command/AttackCommand.cpp
        src/command/FleeCommand.cpp
        src/command/SaveCommand.cpp
        src/command/LoadCommand.cpp
        src/command/DeleteCommand.cpp
        src/command/SavesCommand.cpp
    )
    
    target_include_directories(mud_tests PRIVATE include)
    target_link_libraries(mud_tests PRIVATE 
        GTest::gtest_main 
        GTest::gmock_main
        Threads::Threads
    )
    
    # Windows特定设置
    if(WIN32)
        target_compile_definitions(mud_tests PRIVATE _WIN32_WINNT=0x0601)
        target_link_libraries(mud_tests PRIVATE ws2_32)
    endif()
    
    # 注册测试
    include(GoogleTest)
    gtest_discover_tests(mud_tests
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DISCOVERY_TIMEOUT 60
    )
    
    # 设置测试属性
    add_custom_target(run_tests
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        DEPENDS mud_tests
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running all unit tests..."
    )
endif()
