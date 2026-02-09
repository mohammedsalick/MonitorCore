@echo off
echo Building System Monitor...

if not exist build mkdir build
cd build

echo Running CMake...
cmake ..

if %ERRORLEVEL% NEQ 0 (
    echo CMake failed. Trying manual compilation with MSVC...
    cd ..
    
    REM Try to find MSVC compiler
    where cl >nul 2>&1
    if %ERRORLEVEL% EQU 0 (
        echo Compiling with MSVC...
        cl /EHsc /std:c++17 /Iinclude src\*.cpp /link pdh.lib psapi.lib wbemuuid.lib comsuppw.lib iphlpapi.lib ws2_32.lib /OUT:monitor.exe
        if %ERRORLEVEL% EQU 0 (
            echo Build successful! monitor.exe created.
        ) else (
            echo Build failed with MSVC.
        )
    ) else (
        echo MSVC compiler not found. Please install Visual Studio Build Tools or Visual Studio.
        echo Alternatively, ensure CMake is properly configured.
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
