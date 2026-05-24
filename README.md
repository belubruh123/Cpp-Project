

## **This project is going to be a code gallery platform. And I am planning to add the following games:**


##BIG NOTICE! THE MAIN MENU IS SPECIALLY DESIGNED BY AI! I do code the class and thing but the design and the cursor handling is provided by AI (Gemini).

## *Games Showcase:*

 - 2048
 - Wordle
## *Algorithm Showcase:*

 - Neuron Network Math training
 ***

## Advertising (of course I need to):
 -[My YouTube channel](https://youtube.com/@mgbgame)
 -[My website](https://www.mgb-yt.com) and the useful tool in it

***

## Tutorial on how to build from source your self

***
We have transitioned to using CMake for building this project. Please ensure you have CMake installed.

### **Windows**
**Dependencies**:
- CMake
- MinGW-w64 (GCC/G++) installed and added to your system PATH

**To compile from source:**
```bat
mkdir build
cd build
cmake ..
cmake --build .
```

**Running the app**:
Make sure you run the app from the root project folder so it can locate the required data files (like the Wordle wordlist)!
```bat
cd ..
build\app.exe
```

### **Linux / macOS**
**Dependencies**:
- CMake
- GCC/G++ or Clang
- Required system libraries for Raylib (on Linux: `libasound2-dev`, `libx11-dev`, `libxrandr-dev`, `libxi-dev`, `libgl1-mesa-dev`, `libglu1-mesa-dev`, `libxcursor-dev`, `libxinerama-dev`).

**To compile from source:**
```bash
mkdir build
cd build
cmake ..
make
```

**Running the app**:
Make sure you run the app from the root project folder so it can locate the required data files (like the Wordle wordlist)!
```bash
cd ..
./build/app
```
