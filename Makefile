CC=clang
BUILD_DIR=build
SRC_DIR=src
CCFLAGS=-std=c99 -g -Wall -Werror -fsanitize=address



all: $(BUILD_DIR)/main

$(BUILD_DIR)/main: $(SRC_DIR)/*.c | $(BUILD_DIR)
	$(CC) $(CCFLAGS) -o $@ $^


$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)/*


.PHONY: all clean