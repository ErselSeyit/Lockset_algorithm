# 🔒 Lockset Algorithm - Eraser Data Race Detector

> A professional C++ implementation of the Eraser lockset algorithm for dynamic data race detection in multi-threaded programs. Detects potential race conditions by tracking lock acquisition patterns and shared variable access.

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Project Structure](#project-structure)
- [Prerequisites](#prerequisites)
- [Building](#building)
- [Usage](#usage)
- [Architecture](#architecture)
- [Examples](#examples)
- [License](#license)

## 🎯 Overview

This project implements a dynamic data race detector that uses the lockset algorithm (Eraser algorithm) to detect potential data races in multi-threaded C++ programs. The detector monitors thread access patterns to shared variables and verifies that all accesses are properly protected by locks.

The Eraser algorithm works by:
1. Maintaining a lockset for each thread (set of locks currently held)
2. Tracking access patterns to shared variables
3. Detecting when concurrent accesses occur without common locks
4. Reporting potential data races

## ✨ Features

- **Lockset Algorithm**: Full implementation of the Eraser algorithm for data race detection
- **Thread Tracking**: Monitors thread access patterns and lock acquisitions
- **State Management**: Tracks shared variable states (Virgin, Exclusive, Shared, etc.)
- **Race Detection**: Identifies concurrent accesses without proper synchronization
- **Statistics**: Provides detailed statistics on accesses, locks, and detected races
- **Error Handling**: Comprehensive null pointer checks and error reporting
- **Professional Code**: Well-documented, clean, and maintainable codebase

## 📁 Project Structure

```
Lockset_algorithm/
├── include/              # Header files
│   ├── Accesstype.h
│   ├── DataRaceDetector.h
│   ├── Lock.h
│   ├── SharedVariable.h
│   └── Thread.h
├── src/                 # Source files
│   ├── DataRaceDetector.cpp
│   ├── Lock.cpp
│   ├── main.cpp
│   ├── SharedVariable.cpp
│   └── Thread.cpp
├── examples/            # Example and test programs
│   ├── barrier.cpp
│   ├── benchmark.cpp
│   ├── bigTest.cpp
│   ├── giantTest.cpp
│   ├── r_r_example.cpp
│   ├── read_write_ex.cpp
│   └── w_w_example.cpp
├── Makefile            # Build configuration
├── README.md           # This file
└── LICENSE             # License file
```

## 🔧 Prerequisites

- **C++11 or later** compiler (g++ or clang++)
- **pthread library** (usually included with the compiler)
- **Make** (optional, for using Makefile)

### Platform-Specific Notes

- **Linux/Unix**: pthread is typically included
- **Windows (MinGW/MSYS2)**: pthread is available via MinGW-w64
- **macOS**: pthread is included in the system

## 🏗️ Building

### Using Makefile

```bash
# Build the main program
make

# Build all examples
make examples

# Clean build artifacts
make clean

# Build specific example
make barrier
make benchmark
```

**Note**: If `make` is not available on Windows, you can install it via:
- **MinGW/MSYS2**: `pacman -S make`
- **Chocolatey**: `choco install make`
- **WSL**: Use the Linux version

### Manual Compilation

If `make` is not available, you can compile manually:

```bash
# Main program
g++ -std=c++11 -pthread -I./include src/main.cpp src/DataRaceDetector.cpp src/Lock.cpp src/Thread.cpp src/SharedVariable.cpp -o main

# Example: Build read_write_ex
g++ -std=c++11 -pthread -I./include examples/read_write_ex.cpp src/DataRaceDetector.cpp src/Lock.cpp src/Thread.cpp src/SharedVariable.cpp -o examples/read_write_ex
```

## 🚀 Usage

### Basic Usage

```cpp
#include "../include/DataRaceDetector.h"
#include "../include/Lock.h"
#include "../include/SharedVariable.h"
#include "../include/Thread.h"
#include "../include/Accesstype.h"

int main() {
    // Create detector and resources
    DataRaceDetector drd;
    Lock lock1(1);
    SharedVariable var1("var1");

    drd.locksetMainStart();

    // Register and use locks properly
    Thread thread(1);
    drd.registerThread(&thread);
    drd.registerSharedVariable(&var1);
    
    // Acquire lock before access
    drd.onLockAcquire(&thread, &lock1, true, &var1);
    drd.onSharedVariableAccess(&thread, &var1, AccessType::WRITE);
    drd.onLockRelease(&thread, &lock1, &var1);

    drd.locksetMainEnd();

    // Print statistics
    std::cout << "Data races detected: " << drd.getNumDataRaces() << std::endl;
    
    return 0;
}
```

### Running the Main Program

```bash
./main
```

The program demonstrates two scenarios:
1. **Correct usage**: Proper lock usage preventing data races
2. **Incorrect usage**: Missing locks leading to detected data races

## 🏛️ Architecture

### Core Classes

#### DataRaceDetector
The main detector class implementing the lockset algorithm. It:
- Tracks registered threads and shared variables
- Monitors lock acquisitions and releases
- Detects data races based on lockset intersections
- Provides statistics on detected races

#### Thread
Represents a thread and maintains:
- **Lockset**: Set of all locks currently held
- **Write Lockset**: Set of locks held in write mode
- Thread ID for identification

#### Lock
Represents a synchronization lock that:
- Tracks which thread currently holds it
- Associates with shared variables it protects
- Supports read/write mode tracking

#### SharedVariable
Represents a shared variable with:
- **State tracking**: Virgin, Exclusive, Shared, SharedModified, etc.
- **Access history**: Which thread is currently accessing
- **Candidate locks**: Locks that protect this variable

#### AccessType
Enumeration for access operations:
- `READ`: Read access operation
- `WRITE`: Write access operation

### State Machine

Shared variables transition through states:
- **Virgin**: Never accessed
- **Initializing**: First write access
- **Exclusive**: Single thread has exclusive access
- **Shared**: Multiple threads reading
- **SharedModified**: Multiple threads with at least one write
- **Clean**: Reset state

## 📚 Examples

The `examples/` directory contains several demonstration programs:

- **read_write_ex.cpp**: Basic read/write access patterns
- **r_r_example.cpp**: Read-read scenarios
- **w_w_example.cpp**: Write-write scenarios
- **barrier.cpp**: Barrier synchronization examples
- **benchmark.cpp**: Performance benchmarking
- **bigTest.cpp**: Large-scale test scenarios
- **giantTest.cpp**: Extensive stress testing

To build and run an example:

```bash
make read_write_ex
./examples/read_write_ex
```

## 🔍 Algorithm Details

The lockset algorithm detects data races by:

1. **Maintaining Locksets**: Each thread maintains a set of locks it currently holds
2. **Tracking Accesses**: When a thread accesses a shared variable, the detector checks:
   - Is another thread currently accessing the same variable?
   - Do both threads have at least one common lock?
3. **Race Detection**: If concurrent access occurs without common locks, a data race is reported

### Lockset Intersection

The algorithm checks for common locks between threads using:
- Regular lockset intersection
- Write lockset intersection
- Cross-intersection (one thread's write lockset with another's regular lockset)

## 📊 Statistics

The detector provides the following statistics:
- `getNumAccesses()`: Total number of variable accesses
- `getNumLockAcquisitions()`: Total lock acquisitions
- `getNumLockReleases()`: Total lock releases
- `getNumDataRaces()`: Total data races detected

## 🐛 Error Handling

The implementation includes comprehensive error handling:
- Null pointer checks for all public methods
- Barrier initialization validation
- Lock ownership verification
- Error messages via `std::cerr`

## 📝 License

See LICENSE file for details.

## 🤝 Contributing

This is a professional implementation suitable for:
- Educational purposes
- Research in concurrent programming
- Integration into larger projects
- Learning data race detection algorithms

## 📖 References

- **Eraser: A Dynamic Data Race Detector for Multithreaded Programs** by Stefan Savage, Michael Burrows, Greg Nelson, Patrick Sobalvarro, and Thomas Anderson (1997)

## 🔗 Related Work

The Eraser algorithm is a foundational technique in:
- Dynamic analysis tools
- Thread sanitizers
- Concurrent program verification
- Data race detection research

---

**Note**: This implementation is designed for educational and research purposes. For production use, consider established tools like ThreadSanitizer (TSan) or Helgrind.
// Optimized for thread-safety and race condition detection
