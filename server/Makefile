# FIX-HUB SERVER Makefile
CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall \
    -I./include \
    -I../common/include \
    -I../common/nlohmann

# 掃描 server/src 所有 cpp
SERVER_SRC = $(wildcard src/*.cpp)

# 掃描 common/src 所有 cpp（共用邏輯）
COMMON_SRC = $(wildcard ../common/src/*.cpp)

TARGET = server

all: $(TARGET)

$(TARGET): $(SERVER_SRC) $(COMMON_SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SERVER_SRC) $(COMMON_SRC)

run:
	./fix_server ../config/config.json

clean:
	rm -f $(TARGET)