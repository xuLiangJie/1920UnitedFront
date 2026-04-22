@echo off
echo ========================================================
echo Testing New Unit Tests for Iteration Modules
echo ========================================================
echo.

REM 检查是否在build目录
if not exist "CMakeLists.txt" (
    echo ERROR: Please run this script from the project root directory
    exit /b 1
)

REM 创建build目录
if not exist "build" mkdir build
cd build

echo [1/4] Configuring with CMake...
cmake .. -DMUD_ENABLE_TESTS=ON -DMUD_ENABLE_STRICT_WARNINGS=OFF
if %ERRORLEVEL% neq 0 (
    echo ERROR: CMake configuration failed
    cd ..
    exit /b 1
)
echo.

echo [2/4] Building project...
cmake --build . --config Release --parallel
if %ERRORLEVEL% neq 0 (
    echo ERROR: Build failed
    cd ..
    exit /b 1
)
echo.

echo [3/4] Running new tests only...
echo ========================================================
echo Running EventBus Tests...
echo ========================================================
mud_tests --gtest_filter="EventBusTest.*"
if %ERRORLEVEL% neq 0 (
    echo WARNING: Some EventBus tests failed
)
echo.

echo ========================================================
echo Running CommandRegistry Extended Tests...
echo ========================================================
mud_tests --gtest_filter="CommandRegistryTest.*"
if %ERRORLEVEL% neq 0 (
    echo WARNING: Some CommandRegistry tests failed
)
echo.

echo ========================================================
echo Running CombatManager Tests...
echo ========================================================
mud_tests --gtest_filter="CombatManagerTest.*"
if %ERRORLEVEL% neq 0 (
    echo WARNING: Some CombatManager tests failed
)
echo.

echo [4/4] Running all tests for verification...
echo ========================================================
echo Running Complete Test Suite
echo ========================================================
ctest --output-on-failure
if %ERRORLEVEL% neq 0 (
    echo ERROR: Some tests failed
    cd ..
    exit /b 1
)
echo.

cd ..
echo ========================================================
echo All tests completed successfully!
echo ========================================================
echo.
echo Test Summary:
echo - EventBus Tests: Verifies event publish/subscribe functionality
echo - CommandRegistry Extended Tests: Verifies command registration and execution
echo - CombatManager Tests: Verifies combat system with event-driven architecture
echo.
echo These tests ensure behavior consistency after refactoring.
echo.
exit /b 0
