CC = mpicc
CFLAGS = -std=c99 -Wall -W -fopenmp
SRC_DIR = src
BUILD_DIR = build

all: main readme test

main: build_dir
	@echo "\nBuilding...\n"
	@$(CC) $(CFLAGS) $(SRC_DIR)/main.c -o $(BUILD_DIR)/main
	
build_dir:
	@mkdir -p $(BUILD_DIR)

readme: main
	@echo "Creating readme file...\n"
	@$(BUILD_DIR)/main -h > README.txt

test: main
	@echo "Running tests..\n"
	@$(BUILD_DIR)/main -t
