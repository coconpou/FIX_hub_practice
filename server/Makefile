# Makefile for FixServer

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -fPIC -Wall

# --- Qt6 Configuration ---
# Use pkg-config to get flags, making it portable
QT_CFLAGS = $(shell pkg-config --cflags Qt6Core Qt6Network)
QT_LIBS = $(shell pkg-config --libs Qt6Core Qt6Network)
MOC = $(shell pkg-config --variable=moc Qt6Core)

# --- Project Configuration ---
TARGET = server
BUILD_DIR = build

# Directories
SRC_DIR = src
COMMON_DIR = ../common
VPATH = $(SRC_DIR) $(COMMON_DIR)

# Source files
# Automatically find all .cpp files in specified directories
SOURCES = $(notdir $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(COMMON_DIR)/*.cpp))

# Headers that require MOC processing
MOC_HEADERS = $(SRC_DIR)/FixServer.h

# Object files
# Place all object files in the BUILD_DIR
OBJECTS = $(addprefix $(BUILD_DIR)/, $(SOURCES:.cpp=.o))
MOC_SOURCES = $(addprefix $(BUILD_DIR)/, $(notdir $(MOC_HEADERS:.h=.moc.cpp)))
MOC_OBJECTS = $(MOC_SOURCES:.cpp=.o)

# Include paths
INCLUDES = -I$(SRC_DIR) -I$(COMMON_DIR)

# Final flags
CXXFLAGS += $(QT_CFLAGS) $(INCLUDES)
LIBS = $(QT_LIBS)

.PHONY: all clean

all: $(BUILD_DIR)/$(TARGET)

# --- Build Rules ---

# Linking the final executable
$(BUILD_DIR)/$(TARGET): $(OBJECTS) $(MOC_OBJECTS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) -o $@ $^ $(LIBS)
	@echo "✅ Server build complete: $(BUILD_DIR)/$(TARGET)"

# Compiling .cpp files
# This rule uses VPATH to find the source file
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Generating .moc.cpp files
$(BUILD_DIR)/%.moc.cpp: %.h
	@mkdir -p $(BUILD_DIR)
	$(MOC) $< -o $@

# Compiling .moc.cpp files
$(BUILD_DIR)/%.moc.o: %.moc.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --- Housekeeping ---

clean:
	@echo "Cleaning up server build files..."
	@rm -rf $(BUILD_DIR)
