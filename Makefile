# Makefile for Lockset Algorithm - Eraser Data Race Detector
# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pthread
INCLUDES = -I./include
SRC_DIR = src
EXAMPLES_DIR = examples
BUILD_DIR = build

# Source files
CORE_SOURCES = $(SRC_DIR)/DataRaceDetector.cpp \
               $(SRC_DIR)/Lock.cpp \
               $(SRC_DIR)/Thread.cpp \
               $(SRC_DIR)/SharedVariable.cpp

MAIN_SOURCE = $(SRC_DIR)/main.cpp
MAIN_TARGET = main

# Example files
EXAMPLES = barrier benchmark bigTest giantTest r_r_example read_write_ex w_w_example
EXAMPLE_SOURCES = $(addprefix $(EXAMPLES_DIR)/, $(addsuffix .cpp, $(EXAMPLES)))
EXAMPLE_TARGETS = $(addprefix $(EXAMPLES_DIR)/, $(EXAMPLES))

# Default target
all: $(MAIN_TARGET)

# Main program
$(MAIN_TARGET): $(MAIN_SOURCE) $(CORE_SOURCES)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(MAIN_SOURCE) $(CORE_SOURCES) -o $(MAIN_TARGET)
	@echo "Build complete: $(MAIN_TARGET)"

# Build all examples
examples: $(EXAMPLE_TARGETS)
	@echo "All examples built successfully"

# Individual example targets
$(EXAMPLES_DIR)/barrier: $(EXAMPLES_DIR)/barrier.cpp $(CORE_SOURCES)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(EXAMPLES_DIR)/barrier.cpp $(CORE_SOURCES) -o $(EXAMPLES_DIR)/barrier

$(EXAMPLES_DIR)/benchmark: $(EXAMPLES_DIR)/benchmark.cpp $(CORE_SOURCES)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(EXAMPLES_DIR)/benchmark.cpp $(CORE_SOURCES) -o $(EXAMPLES_DIR)/benchmark

$(EXAMPLES_DIR)/bigTest: $(EXAMPLES_DIR)/bigTest.cpp $(CORE_SOURCES)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(EXAMPLES_DIR)/bigTest.cpp $(CORE_SOURCES) -o $(EXAMPLES_DIR)/bigTest

$(EXAMPLES_DIR)/giantTest: $(EXAMPLES_DIR)/giantTest.cpp $(CORE_SOURCES)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(EXAMPLES_DIR)/giantTest.cpp $(CORE_SOURCES) -o $(EXAMPLES_DIR)/giantTest

$(EXAMPLES_DIR)/r_r_example: $(EXAMPLES_DIR)/r_r_example.cpp $(CORE_SOURCES)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(EXAMPLES_DIR)/r_r_example.cpp $(CORE_SOURCES) -o $(EXAMPLES_DIR)/r_r_example

$(EXAMPLES_DIR)/read_write_ex: $(EXAMPLES_DIR)/read_write_ex.cpp $(CORE_SOURCES)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(EXAMPLES_DIR)/read_write_ex.cpp $(CORE_SOURCES) -o $(EXAMPLES_DIR)/read_write_ex

$(EXAMPLES_DIR)/w_w_example: $(EXAMPLES_DIR)/w_w_example.cpp $(CORE_SOURCES)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(EXAMPLES_DIR)/w_w_example.cpp $(CORE_SOURCES) -o $(EXAMPLES_DIR)/w_w_example

# Clean build artifacts
clean:
	rm -f $(MAIN_TARGET) $(MAIN_TARGET).exe
	rm -f $(EXAMPLE_TARGETS)
	rm -f $(addsuffix .exe, $(EXAMPLE_TARGETS))
	rm -f *.o $(SRC_DIR)/*.o $(EXAMPLES_DIR)/*.o
	rm -f a.out
	@echo "Clean complete"

# Run main program
run: $(MAIN_TARGET)
	./$(MAIN_TARGET)

# Windows-specific targets
.PHONY: windows
windows: CXXFLAGS += -static-libgcc -static-libstdc++
windows: $(MAIN_TARGET)

# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: $(MAIN_TARGET)

# Release build
release: CXXFLAGS += -O3 -DNDEBUG
release: $(MAIN_TARGET)

# Help target
help:
	@echo "Available targets:"
	@echo "  all          - Build main program (default)"
	@echo "  examples     - Build all example programs"
	@echo "  clean        - Remove all build artifacts"
	@echo "  run          - Build and run main program"
	@echo "  debug        - Build with debug symbols"
	@echo "  release      - Build optimized release version"
	@echo "  help         - Show this help message"
	@echo ""
	@echo "Individual example targets:"
	@echo "  barrier, benchmark, bigTest, giantTest,"
	@echo "  r_r_example, read_write_ex, w_w_example"

.PHONY: all examples clean run debug release help windows

