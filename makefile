export PATH := ../llvm-project/build/bin:$(PATH)
BD_DIR := ../llvm-project/build
CONFIG_FLAGS := $(shell $(BD_DIR)/bin/llvm-config --cxxflags --ldflags)
INCLUDE := ../llvm-project/clang/include
TOOL_INCLUDE := $(BD_DIR)/tools/clang/include
CXX := clang++
CXXFLAGS = -Wall -g $(CONFIG_FLAGS) -lncurses -lclang-cpp -lclangTooling -I$(INCLUDE) -I$(TOOL_INCLUDE)

OBJS = example.o
TARGET = example

all: $(TARGET)

aut:
	$(CC) test_func_loop.c       -o test_func_loop
	$(CC) test_bare_loop.c       -o test_bare_loop

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) 


clean:
	rm -rf $(TARGET) $(OBJS)
	rm -rf test_func_loop test_bare_loop

