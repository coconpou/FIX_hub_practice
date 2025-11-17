# FIX-HUB CLIENT Makefile
CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall \
    -I./include \
    -I../common/include \
    -I../common/nlohmann

# 掃描 client/src 所有 cpp
CLIENT_SRC = $(wildcard src/*.cpp)

# 掃描 common/src 所有 cpp（共用邏輯）
COMMON_SRC = $(wildcard ../common/src/*.cpp)

TARGET = fix_client

all: $(TARGET)

$(TARGET): $(CLIENT_SRC) $(COMMON_SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(CLIENT_SRC) $(COMMON_SRC)

run:
	./fix_client ../config/config.json

clean:
	rm -f $(TARGET)