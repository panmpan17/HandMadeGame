.PHONY: compile clean

BUILD_DIR := cmake-build
CMAKE_BIN := ${BUILD_DIR}/bin
OUTPUT_NAME := GLFWTest

compile:
	@echo "Building the project..."
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake .. && make
	@echo "Build complete."
	@cd ..

quick: compile
	@mkdir -p $(CMAKE_BIN)/lib
	@cp -r lib/mac/*dylib $(CMAKE_BIN)/lib
	@cp -r ./assets $(CMAKE_BIN)
	@./$(CMAKE_BIN)/$(OUTPUT_NAME)

mac-build: compile
	@mkdir -p $(CMAKE_BIN)/$(OUTPUT_NAME).app/Contents/MacOS

	cp $(CMAKE_BIN)/$(OUTPUT_NAME) $(CMAKE_BIN)/$(OUTPUT_NAME).app/Contents/MacOS/mac
	mkdir -p $(CMAKE_BIN)/$(OUTPUT_NAME).app/Contents/MacOS/lib
	cp ./lib/mac/*.dylib $(CMAKE_BIN)/$(OUTPUT_NAME).app/Contents/MacOS/lib
	cp -r ./assets $(CMAKE_BIN)/$(OUTPUT_NAME).app/Contents/MacOS/

	mkdir -p $(CMAKE_BIN)/$(OUTPUT_NAME).app/Contents/Resources
	cp MacAppPack/icon-windowed.icns $(CMAKE_BIN)/$(OUTPUT_NAME).app/Contents/Resources/

	cp MacAppPack/info.plist $(CMAKE_BIN)/$(OUTPUT_NAME).app/Contents/

	open -R $(CMAKE_BIN)/$(OUTPUT_NAME).app

clean:
	@echo "Cleaning up..."
	rm -rf $(BUILD_DIR)
