# GLFW Test Project


### Download GLFW Pre-compiled

1. Download [Pre-compiled GLWF](https://www.glfw.org/download.html)
2. make dir under this project folder
3. copy the target platform's to the lib folder
    * If mac copy "libglfw.3.dylib" under /lib/mac/
    * If window copy glfw3.dll, and other to /lib (if mingw use lib-mingw-w64 under release folder)
4. Execute "make compile" to compile the project

### Compile FreeType

Download Version 2.14.1
```bash
curl -LO https://download.savannah.gnu.org/releases/freetype/freetype-2.14.1.tar.gz
tar xzf freetype-2.14.1.tar.gz
cd freetype-2.14.1
```

Build static library
```bash
mkdir build && cd build

# Configure CMake for dinamic library
# Note: This project CMake will use /usr/local/freetype to find its dinamic lib, the path need to be exactly the same
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=ON \
  -DCMAKE_INSTALL_PREFIX=/usr/local/freetype

make -j8
make install
```

