CC = mpicc
CFLAGS = -std=c99 -Wall -W
CFLAGS_END = -lm
SRC_DIR = src
BUILD_DIR = build

all: main readme

main: build_dir
	$(CC) $(CFLAGS) $(SRC_DIR)/main.c -o $(BUILD_DIR)/main $(CFLAGS_END)

build_dir:
	mkdir -p $(BUILD_DIR)

readme: main
	$(BUILD_DIR)/main -h > README.txt
