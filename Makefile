CC=gcc
BUILD_DIR=build
SRC_DIR=src

always: main

main:
	$(CC) -o build/main $(SRC_DIR)/main.c $(SRC_DIR)/tree.c -g -Wall
	valgrind build/main --leak-check=full

clean:
	rm -rf $(BUILD_DIR)/*