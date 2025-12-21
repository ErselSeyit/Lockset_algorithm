#!/bin/bash
# Build script for Unix/Linux systems
# Alternative to Makefile for systems without make

CXX=g++
CXXFLAGS="-std=c++11 -Wall -Wextra -pthread"
INCLUDES="-I./include"
CORE_SOURCES="src/DataRaceDetector.cpp src/Lock.cpp src/Thread.cpp src/SharedVariable.cpp"

case "$1" in
    clean)
        echo "Cleaning build artifacts..."
        rm -f main main.exe
        rm -f *.o src/*.o examples/*.o
        rm -f examples/barrier examples/benchmark examples/bigTest
        rm -f examples/giantTest examples/r_r_example examples/read_write_ex
        rm -f examples/w_w_example
        rm -f examples/*.exe
        echo "Clean complete."
        ;;
    examples)
        echo "Building all examples..."
        $CXX $CXXFLAGS $INCLUDES examples/read_write_ex.cpp $CORE_SOURCES -o examples/read_write_ex
        $CXX $CXXFLAGS $INCLUDES examples/barrier.cpp $CORE_SOURCES -o examples/barrier
        $CXX $CXXFLAGS $INCLUDES examples/benchmark.cpp $CORE_SOURCES -o examples/benchmark
        $CXX $CXXFLAGS $INCLUDES examples/bigTest.cpp $CORE_SOURCES -o examples/bigTest
        $CXX $CXXFLAGS $INCLUDES examples/giantTest.cpp $CORE_SOURCES -o examples/giantTest
        $CXX $CXXFLAGS $INCLUDES examples/r_r_example.cpp $CORE_SOURCES -o examples/r_r_example
        $CXX $CXXFLAGS $INCLUDES examples/w_w_example.cpp $CORE_SOURCES -o examples/w_w_example
        echo "All examples built successfully."
        ;;
    *)
        echo "Building main program..."
        $CXX $CXXFLAGS $INCLUDES src/main.cpp $CORE_SOURCES -o main
        if [ $? -eq 0 ]; then
            echo "Build complete: main"
            echo "Run with: ./main"
        else
            echo "Build failed!"
            exit 1
        fi
        ;;
esac

