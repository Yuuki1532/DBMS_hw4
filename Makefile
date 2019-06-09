CC=g++
CFLAGS=-std=c++11 -Iinclude -Wall -Wno-sign-compare -O2 -pthread
TARGET=shell
DEPS=$(wildcard *.h)

SRC_DIR=src
SRC=$(wildcard $(SRC_DIR)/*.cpp)
OBJ=$(patsubst $(SRC_DIR)/%.cpp, $(SRC_DIR)/%.o, $(SRC))
DEP_SRC=$(filter-out src/$(TARGET).cpp, $(SRC))
DEP_OBJ=$(filter-out src/$(TARGET).o, $(OBJ))

all: $(TARGET)

%.o: %.cpp $(DEPS)
	$(CC) -o $@ $< -c $(CFLAGS)

shell: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	-rm $(TARGET) $(OBJ) $(TEST) $(wildcard test/*.db)
	rm -rf test/system/output/*
