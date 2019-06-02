CC=g++
CFLAGS=-std=c++11 -Iinclude -Wall -g
TARGET=shell
DEPS=$(wildcard *.h)

SRC_DIR=src
SRC=$(wildcard $(SRC_DIR)/*.c)
OBJ=$(patsubst $(SRC_DIR)/%.c, $(SRC_DIR)/%.o, $(SRC))
DEP_SRC=$(filter-out src/$(TARGET).c, $(SRC))
DEP_OBJ=$(filter-out src/$(TARGET).o, $(OBJ))

all: $(TARGET)

%.o: %.c $(DEPS)
	$(CC) -o $@ $< -c $(CFLAGS)

shell: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	-rm $(TARGET) $(OBJ) $(TEST) $(wildcard test/*.db)
	rm -rf test/system/output/*
