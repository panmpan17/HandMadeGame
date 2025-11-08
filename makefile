.PHONY: config-debug compile-debug compile clean build

BUILD_DIR := cmake-build

ifeq ($(OS),Windows_NT)
	CMAKE_BIN := ${BUILD_DIR}\\bin
	ASSET_TIMESTAMP := ${BUILD_DIR}\\assets.timestamp
else
	CMAKE_BIN := ${BUILD_DIR}/bin
	ASSET_TIMESTAMP := ${BUILD_DIR}/assets.timestamp
endif

OUTPUT_NAME := MichaelHandMadeGame

config-debug:
	cmake -S . -B ${BUILD_DIR} -DBUILD_MAC_APP=OFF -DCMAKE_BUILD_TYPE=Debug

compile-debug:
	cmake --build ${BUILD_DIR} --parallel 8

ccquick: config-debug compile-debug
	@echo "Running Debug Build...\n"
	@if ./$(CMAKE_BIN)/$(OUTPUT_NAME); then \
		echo ""; \
	else \
		python3 utilites/check_error.py ${BUILD_DIR}/bin; \
	fi

quick: compile-debug
	@echo "Running Debug Build...\n"
	@if (./$(CMAKE_BIN)/$(OUTPUT_NAME) .); then \
		echo ""; \
	else \
		python3 utilites/check_error.py ${BUILD_DIR}/bin; \
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

clean:
	@echo "Cleaning up..."
	rm -rf $(BUILD_DIR)
