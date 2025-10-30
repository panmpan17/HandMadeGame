.PHONY: config-debug compile-debug compile clean build

BUILD_DIR := cmake-build
CMAKE_BIN := ${BUILD_DIR}/bin
OUTPUT_NAME := MichaelHandMadeGame

config-debug:
	cmake -S . -B ${BUILD_DIR} -DBUILD_MAC_APP=OFF -DCMAKE_BUILD_TYPE=Debug

compile-debug:
	cmake --build ${BUILD_DIR} --parallel 8

ccquick: config-debug compile-debug
	@if ./$(CMAKE_BIN)/$(OUTPUT_NAME); then \
		echo ""; \
	else \
		python3 utilites/check_error.py ${BUILD_DIR}/bin; \
	fi

quick: compile-debug
	@if ./$(CMAKE_BIN)/$(OUTPUT_NAME); then \
		echo ""; \
	else \
		python3 utilites/check_error.py ${BUILD_DIR}/bin; \
	fi

build:
	cmake -S . -B ${BUILD_DIR} -DBUILD_MAC_APP=ON -DCMAKE_BUILD_TYPE=Release
	cmake --build ${BUILD_DIR} --parallel 8
	@open $(BUILD_DIR)/bin/

clean:
	@echo "Cleaning up..."
	rm -rf $(BUILD_DIR)
