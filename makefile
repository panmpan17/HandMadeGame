CXX = g++
SRCS = main.cpp
OUTPUT_NAME = "My GLFW App"
INCLUDES = -I./include
LIBS = -L./lib -lglfw.3
FRAMEWORK = -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo 

compile:
	$(CXX) $(SRCS) -o $(OUTPUT_NAME) $(INCLUDES) $(LIBS) $(FRAMEWORK) -Wl,-rpath,@loader_path/lib -std=c++23

quick: compile
	./$(OUTPUT_NAME)

mac: compile

	mkdir -p $(OUTPUT_NAME).app/Contents/MacOS

	cp $(OUTPUT_NAME) $(OUTPUT_NAME).app/Contents/MacOS/$(OUTPUT_NAME)
	cp -r ./lib $(OUTPUT_NAME).app/Contents/MacOS/

	mkdir -p $(OUTPUT_NAME).app/Contents/Resources
	cp MacAppPack/icon-windowed.icns $(OUTPUT_NAME).app/Contents/Resources/

	cp MacAppPack/info.plist $(OUTPUT_NAME).app/Contents/

clean:
	rm -f $(OUTPUT_NAME)
	rm -rf $(OUTPUT_NAME).app