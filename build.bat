@echo off
REM Build script for Windows
REM Builds the main program and optionally examples

set CXX=g++
set CXXFLAGS=-std=c++11 -pthread -I./include
set CORE_SOURCES=src/DataRaceDetector.cpp src/Lock.cpp src/Thread.cpp src/SharedVariable.cpp

if "%1"=="clean" (
    echo Cleaning build artifacts...
    del /Q main.exe 2>nul
    del /Q *.o 2>nul
    del /Q src\*.o 2>nul
    del /Q examples\*.exe 2>nul
    echo Clean complete.
    exit /b 0
)

if "%1"=="examples" (
    echo Building all examples...
    %CXX% %CXXFLAGS% examples/read_write_ex.cpp %CORE_SOURCES% -o examples/read_write_ex.exe
    %CXX% %CXXFLAGS% examples/barrier.cpp %CORE_SOURCES% -o examples/barrier.exe
    %CXX% %CXXFLAGS% examples/benchmark.cpp %CORE_SOURCES% -o examples/benchmark.exe
    %CXX% %CXXFLAGS% examples/bigTest.cpp %CORE_SOURCES% -o examples/bigTest.exe
    %CXX% %CXXFLAGS% examples/giantTest.cpp %CORE_SOURCES% -o examples/giantTest.exe
    %CXX% %CXXFLAGS% examples/r_r_example.cpp %CORE_SOURCES% -o examples/r_r_example.exe
    %CXX% %CXXFLAGS% examples/w_w_example.cpp %CORE_SOURCES% -o examples/w_w_example.exe
    echo All examples built successfully.
    exit /b 0
)

echo Building main program...
%CXX% %CXXFLAGS% src/main.cpp %CORE_SOURCES% -o main.exe
if %ERRORLEVEL% EQU 0 (
    echo Build complete: main.exe
    echo Run with: main.exe
) else (
    echo Build failed!
    exit /b 1
)

