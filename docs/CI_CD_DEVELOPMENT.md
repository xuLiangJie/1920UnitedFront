# MUD Game - CI/CD 与开发规范文档

## 📋 目录

- [CI/CD 流水线](#cicd-流水线)
- [本地开发](#本地开发)
- [测试框架](#测试框架)
- [API 契约](#api-契约)
- [代码规范](#代码规范)
- [部署流程](#部署流程)

---

## CI/CD 流水线

本项目使用 **GitHub Actions** 实现自动化CI/CD流程，配置文件位于 `.github/workflows/ci.yml`。

### 触发条件

- ✅ Push到 `develop`, `main`, `master` 分支
- ✅ Pull Request到上述分支
- ✅ 手动触发（workflow_dispatch）

### 流水线Job说明

| Job名称 | 运行环境 | 功能 | 状态 |
|---------|---------|------|------|
| **build-and-test** | Windows/Ubuntu (GCC/Clang/MSVC) | 编译项目并运行单元测试 | ✅ 必须通过 |
| **code-quality** | Ubuntu | 静态代码分析（cppcheck） | ⚠️ 警告级别 |
| **integration-test** | Ubuntu | 服务器启动与客户端连接测试 | ✅ 必须通过 |
| **deploy-dry-run** | Ubuntu | 部署验证（仅develop分支） | ℹ️ 信息记录 |

### 编译矩阵

CI会在以下环境组合进行编译测试：

| 操作系统 | 编译器 | 说明 |
|---------|--------|------|
| Windows Latest | MSVC | Windows平台默认编译器 |
| Ubuntu Latest | GCC | Linux平台GCC编译器 |
| Ubuntu Latest | Clang | Linux平台Clang编译器 |

### 状态徽章

```markdown
![CI/CD](https://github.com/<owner>/MudGame/actions/workflows/ci.yml/badge.svg)
```

---

## 本地开发

### 环境要求

- **C++ 编译器**: 支持C++17标准
  - GCC 7+ / Clang 5+ / MSVC 2017+
- **CMake**: 3.15+
- **依赖库**: Boost (特别是 Boost.ASIO)
- **可选**: Google Test (自动下载)

### 编译步骤

```bash
# 1. 克隆仓库
git clone https://github.com/<owner>/MudGame.git
cd MudGame

# 2. 创建构建目录
mkdir build && cd build

# 3. 配置项目（ Release模式）
cmake .. -DCMAKE_BUILD_TYPE=Release

# 4. 编译
cmake --build . --parallel

# 5. (可选) 配置并运行测试
cmake .. -DMUD_ENABLE_TESTS=ON
cmake --build . --target run_tests
```

### 编译选项

| 选项 | 默认值 | 说明 |
|------|--------|------|
| `MUD_ENABLE_TESTS` | OFF | 启用单元测试 |
| `MUD_ENABLE_STRICT_WARNINGS` | OFF | 启用严格警告（警告视为错误） |

### 运行游戏

```bash
# 启动服务器（默认端口8888）
./mud_server [端口]

# 启动客户端
./mud_client [服务器IP] [端口]
```

---

## 测试框架

### 框架选择

- **测试框架**: Google Test (GTest) 1.14.0
- **Mock框架**: Google Mock (GMock)
- **获取方式**: CMake FetchContent自动下载

### 测试文件组织

```
tests/
├── test_player.cpp           # 玩家系统测试
├── test_monster.cpp          # 怪物系统测试
├── test_room.cpp             # 房间与世界测试
├── test_inventory.cpp        # 背包系统测试
├── test_damage_strategy.cpp  # 伤害策略测试
├── test_save_manager.cpp     # 存档系统测试
└── test_commands.cpp         # 命令系统测试
```

### 运行测试

```bash
# 方式1: 使用CMake
cmake --build . --target run_tests

# 方式2: 直接运行测试可执行文件
./mud_tests

# 方式3: 使用ctest
ctest --output-on-failure

# 运行单个测试文件
./mud_tests --gtest_filter="PlayerTest.*"
```

### 测试覆盖率目标

| 模块 | 目标覆盖率 | 当前状态 |
|------|-----------|---------|
| Player | 80% | ✅ 已实现 |
| Monster | 80% | ✅ 已实现 |
| Room/World | 80% | ✅ 已实现 |
| Inventory | 80% | ✅ 已实现 |
| DamageStrategy | 80% | ✅ 已实现 |
| SaveManager | 80% | ✅ 已实现 |
| Commands | 70% | ⚠️ 部分实现 |

### 编写测试用例指南

```cpp
#include <gtest/gtest.h>
#include "your_module.h"

namespace mud {

class YourModuleTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 每个测试前的初始化
    }

    void TearDown() override {
        // 每个测试后的清理
    }
};

TEST_F(YourModuleTest, TestFeature) {
    // 测试逻辑
    EXPECT_EQ(actual, expected);
}

} // namespace mud
```

---

## API 契约

### 协议规范文件

完整的TCP通信协议规范定义在：
- **文件**: `api/mud-game-protocol.yaml`
- **格式**: OpenAPI 3.0.3 (Swagger)
- **用途**: 前后端开发、Mock服务、集成联调的法定依据

### 协议概述

| 属性 | 值 |
|------|-----|
| 传输层 | TCP |
| 默认端口 | 8888 |
| 编码 | UTF-8 |
| 消息分隔 | 换行符 (\n 或 \r\n) |
| 消息格式 | 纯文本命令 |

### 命令分类

| 类别 | 命令列表 |
|------|---------|
| **基础命令** | help, who, look, score, name, say |
| **移动命令** | north/n, south/s, east/e, west/w |
| **战斗命令** | kill, attack, flee |
| **存档命令** | save, load, saves, delete |
| **其他命令** | inventory/i, quit |

### 消息格式示例

#### 请求格式
```
<command> [arg1] [arg2] ...
```

#### 响应格式
```
<格式化文本响应>
```

#### 示例会话
```
客户端: name Arthur
服务器: Your name has been changed from "Stranger" to "Arthur".

客户端: look
服务器: ====== Grand Starting Hall ======
        You stand in the center...

客户端: kill Goblin
服务器: ====== COMBAT STARTED ======
        A Goblin appears before you!
```

### Mock开发

使用OpenAPI规范可以生成Mock服务器用于前端开发：

1. 使用 `swagger-cli` 验证YAML文件
2. 使用 Prism Mock Server 生成Mock服务
3. 前端可以根据规范开发客户端

```bash
# 验证OpenAPI文件
swagger-cli validate api/mud-game-protocol.yaml

# 使用Prism启动Mock服务器（需转换协议）
prism mock api/mud-game-protocol.yaml
```

---

## 代码规范

### 命名约定

| 类型 | 规则 | 示例 |
|------|------|------|
| 类名 | PascalCase | `Player`, `MudServer` |
| 函数名 | camelCase | `getPlayer()`, `setName()` |
| 变量名 | snake_case + 后缀`_` | `name_`, `current_hp_` |
| 常量名 | UPPER_SNAKE_CASE | `MAX_HP`, `DEFAULT_PORT` |
| 命名空间 | lowercase | `mud`, `network` |

### 代码风格

- **缩进**: 4空格
- **大括号**: Allman风格（函数）/ K&R风格（控制流）
- **行长度**: 120字符
- **文件编码**: UTF-8

### 提交规范

遵循 [Conventional Commits](https://www.conventionalcommits.org/) 规范：

```
<type>(<scope>): <description>

[optional body]

[optional footer(s)]
```

**Type类型**:
- `feat`: 新功能
- `fix`: 修复Bug
- `docs`: 文档更新
- `style`: 代码格式
- `refactor`: 重构
- `test`: 测试相关
- `ci`: CI/CD配置

**示例**:
```
feat(combat): add critical hit system

fix(player): fix HP calculation error

docs(api): update OpenAPI protocol spec
```

### 代码审查流程

1. 创建特性分支: `git checkout -b feat/your-feature`
2. 提交代码并编写规范提交信息
3. 推送到远程: `git push origin feat/your-feature`
4. 创建Pull Request
5. **CI自动运行**编译和测试
6. 等待代码审查
7. 审查通过后合并到 `develop`

---

## 部署流程

### 分支策略

```
main/master (生产)
    ↑
develop (开发)
    ↑
feature/* (特性)
```

### 部署步骤

1. **开发阶段**: 在特性分支开发
2. **合并**: PR合并到 `develop`
3. **CI验证**: 自动运行测试和构建
4. **发布**: 从 `develop` 合并到 `main` 发布

### 构建产物

CI会在每次成功后上传构建产物：

| 产物名称 | 内容 | 保留时间 |
|---------|------|---------|
| `windows-latest-msvc-binaries` | Windows可执行文件 | 7天 |
| `ubuntu-latest-gcc-binaries` | Linux可执行文件 | 7天 |
| `ubuntu-latest-clang-binaries` | Linux可执行文件 | 7天 |
| `static-analysis-report` | Cppcheck报告 | 7天 |

### 环境要求

| 环境 | 最低配置 | 推荐配置 |
|------|---------|---------|
| 开发机 | 2核4G | 4核8G |
| 测试服务器 | 2核4G | 4核8G |
| 生产服务器 | 4核8G | 8核16G |

### 监控指标

生产环境应监控：

- 服务器CPU使用率
- 内存使用量
- 活跃连接数
- 命令处理延迟
- 错误率

---

## 附录

### 常见问题

**Q: 测试失败如何排查？**
```bash
# 查看详细输出
ctest --output-on-failure -V

# 运行单个测试
./mud_tests --gtest_filter="PlayerTest.*"
```

**Q: 编译警告如何处理？**
```bash
# 启用严格模式查看所有警告
cmake .. -DMUD_ENABLE_STRICT_WARNINGS=ON
cmake --build .
```

**Q: 如何添加新测试？**
1. 在 `tests/` 目录创建 `test_<module>.cpp`
2. 在 `CMakeLists.txt` 的 `mud_tests` 中添加文件
3. 编写测试用例
4. 运行 `cmake --build . --target run_tests`

### 相关文档

- [README.md](../README.md) - 项目说明
- [REFACTORING_SUMMARY.md](../REFACTORING_SUMMARY.md) - 重构总结
- [api/mud-game-protocol.yaml](../api/mud-game-protocol.yaml) - API契约文件
- [.github/workflows/ci.yml](../.github/workflows/ci.yml) - CI/CD配置

### 联系信息

- **项目维护者**: MUD Game Development Team
- **问题反馈**: GitHub Issues
- **代码审查**: GitHub Pull Requests

---

**最后更新**: 2026年4月15日
