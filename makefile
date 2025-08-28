# --- Colors
GREEN := \033[0;32m
RED := \033[0;31m
YELLOW := \033[0;33m
RESET := \033[0m

# --- General Configuration ---
SRC_DIR := src
BUILD_DIR := build
OUTPUT_FOLDER := output
OUTPUT_NAME := MyGLFWApp

SRCS := $(wildcard $(SRC_DIR)/*.cpp $(SRC_DIR)/**/*.cpp $(SRC_DIR)/**/**/*.cpp include/glad/glad.c)
OBJS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRCS))
OBJS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(OBJS))

# Flags for all platforms
INCLUDES := -I./include
CPP_STD := -std=c++23
WARNING_FLAG :=  -Wall -Wextra -O2
CXXFLAGS := $(CPP_STD) -MMD -MP
RELEASE_BUILD_FLAG := -DDEBUG_FLAG=0
DEBUG_BUILD_FLAG := -DDEBUG_FLAG=1

# --- MacOS Configuration ---
MAC_CXX := g++ # TODO: try use ccache if installed, fallback to g++
MAC_LIBS := -L./lib/mac/ -lglfw.3
MAC_FRAMEWORK := -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo
MAC_RPATH := -Wl,-rpath,@loader_path/lib

# --- Windows Configuration ---
WIN_CXX := g++
WIN_LIBS := -L./lib/ -lglfw3 -lstdc++exp -lopengl32 -lgdi32
WIN_OUTPUT_NAME := $(OUTPUT_NAME).exe


# --- Build Rules ---
$(OUTPUT_FOLDER)/$(OUTPUT_NAME): $(OBJS)
	@echo "Linking $(OUTPUT_NAME)..."
	@mkdir -p $(OUTPUT_FOLDER)
	@$(MAC_CXX) $(OBJS) -o $@ $(MAC_FRAMEWORK) $(MAC_RPATH) $(MAC_LIBS)

# Compile each .cpp/.c into .o
$(BUILD_DIR)/%.o: %.cpp
	@echo "Compiling $<"
	@mkdir -p $(dir $@)
	@$(MAC_CXX) $(CXXFLAGS) $(DEBUG_BUILD_FLAG) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.o: %.c
	@echo "Compiling $<"
	@mkdir -p $(dir $@)
	@$(MAC_CXX) $(CXXFLAGS) $(DEBUG_BUILD_FLAG) $(INCLUDES) -c $< -o $@


# --- Mac Targets ---
mac-compile: $(OUTPUT_FOLDER)/$(OUTPUT_NAME)
	@echo "$(GREEN)Build done -> $(OUTPUT_FOLDER)/$(OUTPUT_NAME)$(RESET)"

mac-quick: mac-compile
	@echo "$(YELLOW)Running $(OUTPUT_NAME)...$(RESET)"
	@mkdir -p $(OUTPUT_FOLDER)/lib
	@cp ./lib/mac/*.dylib $(OUTPUT_FOLDER)/lib
	@cp -r ./assets $(OUTPUT_FOLDER)
	@./$(OUTPUT_FOLDER)/$(OUTPUT_NAME)

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

# --- Windows Targets ---
window-compile:
	@echo "Compiling $(OUTPUT_NAME) for Windows"
	@mkdir -p $(OUTPUT_FOLDER)
	$(WIN_CXX) $(SRCS) $(CPP_STD) -o $(OUTPUT_FOLDER)/$(WIN_OUTPUT_NAME) $(INCLUDES) $(WIN_LIBS) -mwindows

window-quick: window-compile
	@echo "Not implemented for Windows yet"

window-build: window-compile
	cp -r ./assets $(OUTPUT_FOLDER)

# --- Decide platform ---
compile-all: mac-compile window-compile

quick-all: mac-quick window-quick

build-all: mac-build window-build

# --- Others ---
clean:
	rm -rf $(OUTPUT_FOLDER) $(BUILD_DIR)
	@echo "$(GREEN)Cleaned up build files$(RESET)"

.PHONY: mac-compile mac-quick mac-build window-compile window-quick window-build compile-all quick-all build-all clean

-include $(OBJS:.o=.d)