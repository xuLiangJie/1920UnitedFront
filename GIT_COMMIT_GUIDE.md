# Git 提交指南

## 📝 建议的提交策略

由于本次任务涉及多个文件和模块，建议分多次提交，遵循Conventional Commits规范。

---

## 第一次提交：CI/CD配置

```bash
# 添加GitHub Actions配置
git add .github/workflows/ci.yml
git add validate_ci.bat

# 提交
git commit -m "feat(ci): add GitHub Actions CI/CD pipeline

- Configure multi-platform build tests (Windows/Ubuntu)
- Add unit test execution with Google Test
- Add static code analysis with cppcheck
- Add integration test for server startup
- Configure build artifact upload
- Add CI validation script

Refs: #CI/CD setup"
```

---

## 第二次提交：测试框架配置

```bash
# 添加CMakeLists.txt修改和测试文件
git add CMakeLists.txt
git add tests/

# 提交
git commit -m "feat(test): add Google Test framework and unit tests

- Configure Google Test v1.14.0 via FetchContent
- Add 7 test modules with 88+ test cases:
  * Player system (16 tests)
  * Monster system (14 tests)
  * Room & World (17 tests)
  * Inventory system (13 tests)
  * Damage strategies (13 tests)
  * Save manager (12 tests)
  * Command registry (3 tests)
- Add MUD_ENABLE_TESTS CMake option
- Add run_tests custom target

Refs: #testing"
```

---

## 第三次提交：OpenAPI接口契约

```bash
# 添加API契约文档
git add api/mud-game-protocol.yaml

# 提交
git commit -m "docs(api): add OpenAPI 3.0 TCP protocol specification

- Define complete command interface (17 commands)
- Document request/response formats
- Add data model schemas (Player, Item, Room, Monster)
- Include usage examples for all commands
- Provide validation rules and error responses

Refs: #API contract"
```

---

## 第四次提交：文档

```bash
# 添加文档
git add docs/CI_CD_DEVELOPMENT.md
git add DELIVERY_SUMMARY.md
git add REFACTORING_SUMMARY.md

# 提交
git commit -m "docs: add CI/CD development guide and delivery summary

- Add comprehensive CI/CD development guide
- Document testing framework usage instructions
- Add API contract usage guide
- Include delivery summary and validation results

Refs: #documentation"
```

---

## 一次性提交（如果不建议拆分）

```bash
# 添加所有新文件
git add .github/
git add tests/
git add api/
git add docs/CI_CD_DEVELOPMENT.md
git add DELIVERY_SUMMARY.md
git add validate_ci.bat
git add CMakeLists.txt

# 提交
git commit -m "feat(ci): add CI/CD pipeline, test framework, and API contract

CI/CD Pipeline:
- GitHub Actions workflow with multi-platform builds
- Automated unit test execution
- Static code analysis with cppcheck
- Integration testing for server startup
- Build artifact archival

Test Framework:
- Google Test v1.14.0 integration
- 7 test modules with 88+ test cases
- Coverage for core game systems
- CMake configuration with MUD_ENABLE_TESTS option

API Contract:
- OpenAPI 3.0.3 specification for TCP protocol
- Complete command interface documentation
- Data model schemas and examples
- Mock development support

Documentation:
- CI/CD development guide
- Delivery summary and validation results
- Quick start instructions

Refs: #CI/CD #testing #API-contract"
```

---

## 推送并触发CI

```bash
# 推送到远程仓库（会触发CI流水线）
git push origin feature/battle-system

# 或合并到develop后触发
git checkout develop
git merge feature/battle-system
git push origin develop
```

---

## 验证CI状态

1. 访问GitHub仓库
2. 点击 "Actions" 标签
3. 查看 "CI/CD Pipeline" 工作流
4. 点击最新运行查看详情
5. 检查各个Job的执行状态

---

## ⚠️ 注意事项

### 提交前检查清单

- [ ] 所有测试文件都能正常编译
- [ ] 运行`validate_ci.bat`验证通过
- [ ] CMakeLists.txt配置正确
- [ ] OpenAPI YAML文件格式正确
- [ ] 文档无拼写错误

### CI可能失败的原因

1. **Boost库未安装**
   - GitHub Actions Ubuntu镜像包含Boost
   - Windows需要额外配置

2. **网络连接问题**
   - FetchContent下载Google Test可能失败
   - 可以尝试使用cache

3. **编译器版本不兼容**
   - 需要支持C++17的编译器
   - CI配置已包含多版本测试

### 如果CI失败

1. 查看详细错误日志
2. 本地复现问题
3. 修复后重新提交
4. 观察CI是否通过

---

## 📊 提交统计

| 类型 | 数量 | 说明 |
|------|------|------|
| 新增文件 | 12 | CI配置、测试、API、文档 |
| 修改文件 | 1 | CMakeLists.txt |
| 总行数 | ~2000+ | 包含代码、配置、文档 |
| 测试用例 | 88+ | 覆盖7个核心模块 |

---

**建议**: 使用分次提交方式，每次提交专注于一个功能模块，便于代码审查和问题追踪。
