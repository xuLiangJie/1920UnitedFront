@echo off
echo ========================================
echo MUD Game - CI/CD Configuration Validator
echo ========================================
echo.

REM 检查必要文件是否存在
echo [1/5] Checking CI/CD configuration files...

if exist ".github\workflows\ci.yml" (
    echo   [OK] .github/workflows/ci.yml
) else (
    echo   [FAIL] .github/workflows/ci.yml - NOT FOUND
)

echo.
echo [2/5] Checking test framework files...

if exist "tests\test_player.cpp" (
    echo   [OK] tests/test_player.cpp
) else (
    echo   [FAIL] tests/test_player.cpp - NOT FOUND
)

if exist "tests\test_monster.cpp" (
    echo   [OK] tests/test_monster.cpp
) else (
    echo   [FAIL] tests/test_monster.cpp - NOT FOUND
)

if exist "tests\test_room.cpp" (
    echo   [OK] tests/test_room.cpp
) else (
    echo   [FAIL] tests/test_room.cpp - NOT FOUND
)

if exist "tests\test_inventory.cpp" (
    echo   [OK] tests/test_inventory.cpp
) else (
    echo   [FAIL] tests/test_inventory.cpp - NOT FOUND
)

if exist "tests\test_damage_strategy.cpp" (
    echo   [OK] tests/test_damage_strategy.cpp
) else (
    echo   [FAIL] tests/test_damage_strategy.cpp - NOT FOUND
)

if exist "tests\test_save_manager.cpp" (
    echo   [OK] tests/test_save_manager.cpp
) else (
    echo   [FAIL] tests/test_save_manager.cpp - NOT FOUND
)

if exist "tests\test_commands.cpp" (
    echo   [OK] tests/test_commands.cpp
) else (
    echo   [FAIL] tests/test_commands.cpp - NOT FOUND
)

echo.
echo [3/5] Checking API contract files...

if exist "api\mud-game-protocol.yaml" (
    echo   [OK] api/mud-game-protocol.yaml
) else (
    echo   [FAIL] api/mud-game-protocol.yaml - NOT FOUND
)

echo.
echo [4/5] Checking documentation...

if exist "docs\CI_CD_DEVELOPMENT.md" (
    echo   [OK] docs/CI_CD_DEVELOPMENT.md
) else (
    echo   [FAIL] docs/CI_CD_DEVELOPMENT.md - NOT FOUND
)

echo.
echo [5/5] Checking CMakeLists.txt modifications...

findstr /C:"MUD_ENABLE_TESTS" CMakeLists.txt >nul 2>&1
if %errorlevel% equ 0 (
    echo   [OK] CMakeLists.txt contains MUD_ENABLE_TESTS option
) else (
    echo   [FAIL] CMakeLists.txt missing MUD_ENABLE_TESTS option
)

findstr /C:"googletest" CMakeLists.txt >nul 2>&1
if %errorlevel% equ 0 (
    echo   [OK] CMakeLists.txt contains Google Test configuration
) else (
    echo   [FAIL] CMakeLists.txt missing Google Test configuration
)

findstr /C:"mud_tests" CMakeLists.txt >nul 2>&1
if %errorlevel% equ 0 (
    echo   [OK] CMakeLists.txt contains mud_tests target
) else (
    echo   [FAIL] CMakeLists.txt missing mud_tests target
)

echo.
echo ========================================
echo Validation Complete!
echo ========================================
echo.
echo Next steps:
echo 1. Commit and push to trigger CI pipeline
echo 2. Test locally: mkdir build ^&^& cd build
echo 3. Configure: cmake .. -DMUD_ENABLE_TESTS=ON
echo 4. Build: cmake --build . --parallel
echo 5. Run tests: cmake --build . --target run_tests
echo.
pause
