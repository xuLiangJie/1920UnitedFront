@echo off
setlocal

REM 切换到项目根目录
cd /d "%~dp0"

REM 创建或清理build目录
if not exist build mkdir build
cd build

REM 配置CMake并启用测试
cmake .. -DMUD_ENABLE_TESTS=ON -G "MinGW Makefiles"

REM 构建项目（包括测试）
cmake --build .

echo.
echo 构建完成！现在可以运行测试了。
echo 要运行测试，请执行: ctest --output-on-failure
pause