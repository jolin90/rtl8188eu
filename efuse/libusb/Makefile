BUILD_DIR := out

all: clean
	@ mkdir -p $(BUILD_DIR)
	@ cd $(BUILD_DIR) && cmake .. && make

clean:
	@ rm $(BUILD_DIR) -rf
