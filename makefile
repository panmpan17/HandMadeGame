.PHONY: build clean

BUILD_DIR := cmake-build

build:
	@echo "Building the project..."
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake .. && make
	@echo "Build complete."
	@@cd ..

clean:
	@echo "Cleaning up..."
	rm -rf $(BUILD_DIR)
