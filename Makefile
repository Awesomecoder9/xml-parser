CC=gcc
BUILD_DIR=build
SRC_DIR=src



all: $(BUILD_DIR)/main

$(BUILD_DIR)/main: $(SRC_DIR)/*.c | $(BUILD_DIR)
	$(CC) -std=c99 -o $@ $^ -g -Wall -Werror


$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)/*


.PHONY: all clean