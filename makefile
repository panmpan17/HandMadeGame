.PHONY: config-debug compile-debug compile ccquick quick clean build

BUILD_DIR := cmake-build
OUTPUT_NAME := MichaelHandMadeGame

ifeq ($(OS),Windows_NT)
	CMAKE_BIN := ${BUILD_DIR}\\bin
	ASSET_TIMESTAMP := ${BUILD_DIR}\\assets.timestamp
	EXECUTABLE_COMMAND := ./$(BUILD_DIR)/bin/$(OUTPUT_NAME).exe .
else
	CMAKE_BIN := ${BUILD_DIR}/bin
	ASSET_TIMESTAMP := ${BUILD_DIR}/assets.timestamp
	EXECUTABLE_COMMAND := ./$(CMAKE_BIN)/$(OUTPUT_NAME) .
endif

XCODE_PROJECT_FOLDER := xcode_proj

REBUILD ?= 0

config-debug:
	cmake -S . -B ${BUILD_DIR} -DBUILD_APP=OFF -DCMAKE_BUILD_TYPE=Debug

compile-debug:
	cmake --build ${BUILD_DIR} --parallel 8

ccquick: config-debug compile-debug
	@echo "Running Debug Build...\n"
	@echo "Executable Command: $(EXECUTABLE_COMMAND)\n"
	@if ($(EXECUTABLE_COMMAND)); then \
		echo ""; \
	else \
		python3 utilites/check_error.py crash_log.log ${BUILD_DIR}/bin; \
	fi

quick: compile-debug
	@echo "Running Debug Build...\n"
	@if ($(EXECUTABLE_COMMAND)); then \
		echo ""; \
	else \
		python3 utilites/check_error.py crash_log.log ${BUILD_DIR}/bin; \
	fi

build:
	@if [ -f "$(ASSET_TIMESTAMP)" ]; then \
		rm "$(ASSET_TIMESTAMP)"; \
	fi

	cmake -S . -B ${BUILD_DIR} -DBUILD_APP=ON -DCMAKE_BUILD_TYPE=Release
	cmake --build ${BUILD_DIR} --parallel 8

	@echo "Opening build directory..."

ifeq ($(OS),Windows_NT)
	@start "$(CMAKE_BIN)"
else
	@open "$(CMAKE_BIN)"
endif

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
