# Michael's Hand Made Game

A fully custom C++ game engine built from scratch, focusing on rendering, cross-platform builds, and a clean modular architecture.
This project demonstrates my understanding of real-time graphics, engine architecture, and build systems across macOS and Windows.

<!-- Features -->

<!-- 🏗️ Project Architecture -->

```graphql
HandMadeGame/
├── CMakeLists.txt
└── external/
    ├── glfw/         # Window and input handling
    └── imgui/        # ImGui GUI library integration
```

### Depndencies
All dependencies are included as git submodules under external/.
* GLFW
* IMGui

### Build Instructions

**Clone the Repository (with Submodules)**
```
$ git clone --recurse-submodules https://github.com/panmpan17/HandMadeGame.git
```

#### macOS Build
Requirements:
* CMake 3.16
* C++ 23

```bash
$ make build # When the mac application is built, will automatically reveal in Finder
```

####  Windows

Requirements:
* Ninja
* Bash
* C++ 17 (Limited by Mingw)
* make (Optional)

With make
```bash
$ make compile # the exe should be under "cmake-build/bin/"
```

Without make
```bash
$ cmake -S . -B ${BUILD_DIR} -DBUILD_MAC_APP=OFF -DCMAKE_BUILD_TYPE=Debug
$ cmake --build ${BUILD_DIR} --parallel <cpu count>
```

<!-- Future Work -->


### 🧾 License
MIT License — free for learning, modification, and personal use.
