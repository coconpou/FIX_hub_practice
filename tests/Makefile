# Makefile for Unit Tests

# Compiler and flags
CXX = g++
# C++17 for std::filesystem, add -lstdc++fs if needed on older GCC
CXXFLAGS = -std=c++17 -fPIC -Wall 

# --- Qt6 Configuration ---
QT_CFLAGS = $(shell pkg-config --cflags Qt6Core Qt6Network)
QT_LIBS = $(shell pkg-config --libs Qt6Core Qt6Network)

# --- Project Configuration ---
TARGET = run_tests
BUILD_DIR = build

# Directories
COMMON_DIR = ../common
SERVER_SRC_DIR = ../server/src
VPATH = . $(COMMON_DIR) $(SERVER_SRC_DIR)

# Source files
# Test sources
TEST_SOURCES = $(wildcard test_*.cpp) main_test.cpp
# Common library sources
COMMON_SOURCES = $(notdir $(wildcard $(COMMON_DIR)/*.cpp))
# Server sources needed for tests (e.g., FixServer for serialize function)
SERVER_TEST_DEPS = FixServer.cpp

SOURCES = $(TEST_SOURCES) $(COMMON_SOURCES) $(SERVER_TEST_DEPS)

# Object files
OBJECTS = $(addprefix $(BUILD_DIR)/, $(SOURCES:.cpp=.o))

# Include paths
INCLUDES = -I. -I$(COMMON_DIR) -I$(SERVER_SRC_DIR) -I../client/src

# Final flags
CXXFLAGS += $(QT_CFLAGS) $(INCLUDES)
LIBS = $(QT_LIBS)

.PHONY: all clean test

all: $(BUILD_DIR)/$(TARGET)

test: all
	@echo "Running tests..."
	@./$(BUILD_DIR)/$(TARGET)

# --- Build Rules ---

# Linking the final executable
$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) -o $@ $^ $(LIBS)
	@echo "✅ Test build complete: $(BUILD_DIR)/$(TARGET)"

# Compiling .cpp files
# This rule uses VPATH to find the source file
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --- Housekeeping ---

clean:
	@echo "Cleaning up test build files..."
	@rm -rf $(BUILD_DIR)
