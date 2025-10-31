.PHONY: config-debug compile-debug compile ccquick quick clean build

BUILD_DIR := cmake-build
CMAKE_BIN := ${BUILD_DIR}/bin
OUTPUT_NAME := MichaelHandMadeGame
XCODE_PROJECT_FOLDER := xcode_proj

REBUILD ?= 0

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

port-xcode:
	@if [ "$(REBUILD)" -eq "1" ]; then \
		echo "Rebuilding Xcode project..."; \
		rm -rf ${XCODE_PROJECT_FOLDER}; \
	fi
	cmake -S . -B ${XCODE_PROJECT_FOLDER} -G Xcode -DCMAKE_BUILD_TYPE=Debug
	open -R ${XCODE_PROJECT_FOLDER}/${OUTPUT_NAME}.xcodeproj

clean:
	@echo "Cleaning up..."
	rm -rf $(BUILD_DIR)
