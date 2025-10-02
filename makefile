.PHONY: compile clean build

BUILD_DIR := cmake-build
CMAKE_BIN := ${BUILD_DIR}/bin
OUTPUT_NAME := GLFWTest

compile:
	cmake -S . -B ${BUILD_DIR} -DBUILD_MAC_APP=OFF -DCMAKE_BUILD_TYPE=Debug
	cmake --build ${BUILD_DIR} --parallel 8

quick: compile
	@./$(CMAKE_BIN)/$(OUTPUT_NAME)

build:
	cmake -S . -B ${BUILD_DIR} -DBUILD_MAC_APP=ON -DCMAKE_BUILD_TYPE=Release
	cmake --build ${BUILD_DIR} --parallel 8

clean:
	@echo "Cleaning up..."
	rm -rf $(BUILD_DIR)
