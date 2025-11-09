# Basic Configuration
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Iinclude -O2
AR := ar
ARFLAGS := rcs

SRC_DIR := src
INC_DIR := include
TEST_DIR := tests
BIN_DIR := bin
LIB_DIR := lib

$(shell mkdir -p $(BIN_DIR) $(LIB_DIR))

# File Definitions
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(SRC_FILES:$(SRC_DIR)/%.cpp=$(LIB_DIR)/%.o)
LIB_FILE := $(LIB_DIR)/libcore.a

TEST_SRC := $(wildcard $(TEST_DIR)/*.cpp)
TEST_BIN := $(TEST_SRC:$(TEST_DIR)/%.cpp=$(BIN_DIR)/%)

# Build Rules
all: $(LIB_FILE) $(TEST_BIN)
	@echo "✅ Build finished. All executables are in $(BIN_DIR)"

$(LIB_FILE): $(OBJ_FILES)
	$(AR) $(ARFLAGS) $@ $^

$(LIB_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR)/%: $(TEST_DIR)/%.cpp $(LIB_FILE)
	$(CXX) $(CXXFLAGS) $< -L$(LIB_DIR) -lcore -o $@

# Testing
# Run a single test, e.g. make test_calculator
test_%: $(BIN_DIR)/test_%
	@echo "Running $< ..."
	@$<

# Run all tests
test: $(TEST_BIN)
	@echo "Running all tests..."
	@for t in $(TEST_BIN); do echo "$$t"; ./$$t || exit 1; done
	@echo "✅ All tests passed."

# Cleanup

clean:
	rm -rf $(BIN_DIR) $(LIB_DIR)
	@echo "Clean completed."

.PHONY: all clean test test_%