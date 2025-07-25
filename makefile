CXX = g++
SRCS = main.cpp
OUTPUT_NAME = "My GLFW App"
INCLUDES = -I./include
LIBS = -L./lib -lglfw.3
FRAMEWORK = -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo 

compile:
	$(CXX) $(SRCS) -o $(OUTPUT_NAME) $(INCLUDES) $(LIBS) $(FRAMEWORK) -Wl,-rpath,@loader_path/lib