@echo off
echo Building System Monitor...

if not exist build mkdir build
cd build

echo Running CMake...
cmake ..

if %ERRORLEVEL% NEQ 0 (
    echo CMake failed. Trying manual compilation...
    cd ..
    echo Compiling with g++...
    g++ -std=c++17 -o monitor.exe src\*.cpp -Iinclude -lpdh -lpsapi -lwbemuuid -lcomsuppw -liphlpapi
    if %ERRORLEVEL% EQU 0 (
        echo Build successful! monitor.exe created.
    ) else (
        echo Build failed. Please install MinGW or use Visual Studio.
    )
    exit /b
)

echo Building with CMake...
cmake --build .

if %ERRORLEVEL% EQU 0 (
    echo Build successful!
    if exist Debug\monitor.exe (
        echo Executable: build\Debug\monitor.exe
    ) else if exist Release\monitor.exe (
        echo Executable: build\Release\monitor.exe
    ) else if exist monitor.exe (
        echo Executable: build\monitor.exe
    )
) else (
    echo Build failed.
)

cd ..
