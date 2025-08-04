# --- General Configuration ---
SRCS = src/*.cpp src/draw/*.cpp src/components/*.cpp include/glad/glad.c
OUTPUT_FOLDER = output
OUTPUT_NAME = "My GLFW App"

# Flags for all platforms
INCLUDES = -I./include
COMMON_LIBS = 
RELEASE_BUILD_FLAG = -DDEBUG_FLAG=0
DEBUG_BUILD_FLAG = -DDEBUG_FLAG=1
CPP_STD = -std=c++23

# --- MacOS Configuration ---
MAC_CXX = g++
MAC_LIBS = -L./lib/mac/ -lglfw.3

MAC_FRAMEWORK = -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo
MAC_RPATH = -Wl,-rpath,@loader_path/lib


# --- Windows Configuration ---
WIN_CXX = g++

WIN_LIBS = -L./lib/ -lglfw3 -lstdc++exp -lopengl32 -lgdi32
WIN_RPATH = -Wl,-rpath,@loader_path/lib
WIN_OUTPUT_NAME = $(OUTPUT_NAME).exe



mac-compile:
	@echo "Compiling $(OUTPUT_NAME) to $(OUTPUT_FOLDER) for MacOS"
	@mkdir -p $(OUTPUT_FOLDER)
	$(MAC_CXX) $(SRCS) -o $(OUTPUT_FOLDER)/$(OUTPUT_NAME) $(INCLUDES) $(COMMON_LIBS) $(CPP_STD) $(DEBUG_BUILD_FLAG) $(MAC_FRAMEWORK) $(MAC_RPATH) $(MAC_LIBS)

mac-quick: mac-compile
	mkdir -p $(OUTPUT_FOLDER)/lib
	cp ./lib/mac/*.dylib $(OUTPUT_FOLDER)/lib
	cp -r ./assets $(OUTPUT_FOLDER)
	./$(OUTPUT_FOLDER)/$(OUTPUT_NAME)

mac-build: mac-compile

	mkdir -p $(OUTPUT_FOLDER)/$(OUTPUT_NAME).app/Contents/MacOS

	cp $(OUTPUT_FOLDER)/$(OUTPUT_NAME) $(OUTPUT_FOLDER)/$(OUTPUT_NAME).app/Contents/MacOS/$(OUTPUT_NAME)
	mkdir -p $(OUTPUT_FOLDER)/$(OUTPUT_NAME).app/Contents/MacOS/lib
	cp ./lib/mac/*.dylib $(OUTPUT_FOLDER)/$(OUTPUT_NAME).app/Contents/MacOS/lib
	cp -r ./assets $(OUTPUT_FOLDER)/$(OUTPUT_NAME).app/Contents/MacOS/

	mkdir -p $(OUTPUT_FOLDER)/$(OUTPUT_NAME).app/Contents/Resources
	cp MacAppPack/icon-windowed.icns $(OUTPUT_FOLDER)/$(OUTPUT_NAME).app/Contents/Resources/

	cp MacAppPack/info.plist $(OUTPUT_FOLDER)/$(OUTPUT_NAME).app/Contents/

	open -R $(OUTPUT_FOLDER)/$(OUTPUT_NAME).app

window-compile:
	@echo "Compiling $(OUTPUT_NAME) to $(OUTPUT_FOLDER) for Window"
	@mkdir -p $(OUTPUT_FOLDER)
	$(WIN_CXX) $(SRCS) $(CPP_STD) -o $(OUTPUT_FOLDER)/$(OUTPUT_NAME) $(INCLUDES) $(COMMON_LIBS) $(DEBUG_BUILD_FLAG) $(WIN_LIBS) -mwindows

window-quick: window-compile
	@echo "Not implemented for Windows yet"

window-build: window-compile
	cp -r ./assets $(OUTPUT_FOLDER)

compile-all: mac-compile window-compile

quick-all: mac-quick window-quick

build-all: mac-build window-build

clean:
	rm -rf $(OUTPUT_FOLDER)

.PHONY: mac-compile mac-quick mac-build window-compile window-quick window-build compile-all quick-all build-all clean