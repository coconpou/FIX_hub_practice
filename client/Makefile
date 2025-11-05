# Makefile for FixClient

TARGET = fix_client
SRC = main_client.cpp FixClient.cpp
HDR = FixClient.h
OBJS = $(SRC:.cpp=.o)
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I. $(shell pkg-config --cflags Qt6Core Qt6Network)
LDFLAGS = $(shell pkg-config --libs Qt6Core Qt6Network)
BUILD_DIR = build

# Default rule
all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)
	@echo "✅ Build complete: $@"

%.o: %.cpp $(HDR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: all
	./$(BUILD_DIR)/$(TARGET)

clean:
	rm -rf $(BUILD_DIR) *.o

.PHONY: all clean run