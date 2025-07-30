CXX = g++
SRCS = src/main.cpp src/window.cpp include/glad/glad.c src/camera.cpp src/draw/shader.cpp src/file_utils.cpp src/draw/quad.cpp src/draw/triangle.cpp src/input_handle.cpp src/image.cpp
OUTPUT_FOLDER = output
OUTPUT_NAME = "My GLFW App"
INCLUDES = -I./include
LIBS = -L./lib -lglfw.3
FRAMEWORK = -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo 

RELEASE_BUILD_FLAG = -DDEBUG_FLAG=0
DEBUG_BUILD_FLAG = -DDEBUG_FLAG=1

compile:
	@echo "Compiling $(OUTPUT_NAME) to $(OUTPUT_FOLDER)"
	@mkdir -p $(OUTPUT_FOLDER)
	$(CXX) $(SRCS) -o $(OUTPUT_FOLDER)/$(OUTPUT_NAME) $(INCLUDES) $(LIBS) $(FRAMEWORK) -Wl,-rpath,@loader_path/lib -std=c++23 $(DEBUG_BUILD_FLAG)

quick: compile
	cp -r ./lib $(OUTPUT_FOLDER)
	cp -r ./assets $(OUTPUT_FOLDER)
	./$(OUTPUT_FOLDER)/$(OUTPUT_NAME)

mac: compile

	mkdir -p $(OUTPUT_FOLDER)/$(OUTPUT_NAME).app/Contents/MacOS

	cp $(OUTPUT_FOLDER)/$(OUTPUT_NAME) $(OUTPUT_FOLDER)/$(OUTPUT_NAME).app/Contents/MacOS/$(OUTPUT_NAME)
	cp -r ./lib $(OUTPUT_FOLDER)/$(OUTPUT_NAME).app/Contents/MacOS/
	cp -r ./assets $(OUTPUT_FOLDER)/$(OUTPUT_NAME).app/Contents/MacOS/

	mkdir -p $(OUTPUT_FOLDER)/$(OUTPUT_NAME).app/Contents/Resources
	cp MacAppPack/icon-windowed.icns $(OUTPUT_FOLDER)/$(OUTPUT_NAME).app/Contents/Resources/

	cp MacAppPack/info.plist $(OUTPUT_FOLDER)/$(OUTPUT_NAME).app/Contents/

	open -R $(OUTPUT_FOLDER)/$(OUTPUT_NAME).app

clean:
	rm -rf $(OUTPUT_FOLDER)