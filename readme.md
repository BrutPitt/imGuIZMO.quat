# imGuIZMO
**imGuIZMO** is an widget for [**dear ImGui**](https://github.com/ocornut/imgui) (Immediate Mode Graphics User Interface) library. it provides a way to rotate model, light, or object with mouse, and graphically visualize its position in space. 

![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/imGuIZMO.gif)

**imGuIZMO** is written in C++ (C++14) and consist of only two files (*imGuIZMO.h* and *imGuIZMO.cpp*) that you can simply add to your project.
It uses [**virtualGizmo3D**](https://github.com/BrutPitt/virtualGizmo3D): my *header only* 3D GIZMO manipulator tool (in *Immediate Mode* without pan and dolly functions) and [**glm** mathematics library](https://github.com/g-truc/glm) (0.9.9 or higher), also it an *header only* tool.

Three widget types are prvided, (six function calls with different parameters: *quaternion, vec4, vec3* for different uses) each of them personalizable with several graphics options:

### Axes mode
![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/A001.jpg)![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/A002.jpg)![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/A003.jpg)![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/A004.jpg)

### Directional arrow
![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/B001.jpg)![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/B002.jpg)![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/B003.jpg)

### Axes + spot
![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/C001.jpg)![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/C002.jpg)![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/C003.jpg)![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/C004.jpg)

And with full configurable: lenght, thickness, dimensions, number of polygon slices, colors and sphere tesselation:

![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/D001.jpg)![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/D002.jpg)![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/D003.jpg)

### Live WebGL2 demo
You can run/test **WebGL 2** examples of **imGuIZMO** from following links:
- [imGuIZMO ImGui widget manipulator (only)](https://www.michelemorrone.eu/emsExamples/qjSetWidget.html)
- [imGuIZMO ImGui widget + 3D (dolly/zoom) screen manipulator](https://www.michelemorrone.eu/emsExamples/qjSetScreen.html)

There is way to test all available options and settings.
It works only on browsers with **WebGl 2** and *webassembly* support (FireFox/Chrome/Opera)
( test here if your browser supports it: [WebGL 2 Report](http://webglreport.com/?v=2) )


### How to use

In this example I use GLFW and openGL, but it is simple to change this if you use Vulkan/DirectX/etc, SDL/GLUT/etc, or native OS access.

To use imGuIZMO need to include imGuIZMO.h file in your code, then an possible use can be to declare an object of type glm::quat (quaternion), global or as member of your class, to mantain track of rotations:

```cpp
#include "imGuIZMO.h"

/////////////////////////////////////////////////////////////////////////////
// For imGuIZMO, declare global variable or member class quaternion
    glm::quat qRot = glm::quat(1.f, 0.f, 0.f, 0.f);

/////////////////////////////////////////////////////////////////////////////
// two helper functions, not really necessary (but comfortable)
    void setRotation(const glm::quat &q) { qRot = q; }
    glm::quat& getRotation() { return qRot; }
 ```

In your ImGui window, for a 3 axes widget, simply:
```cpp
    glm::quat qt = getRotation();
    if(ImGui::gizmo3D("##gizmo1", qt /*, size,  mode */)) {  setRotation(qt); }

    // Default size: ImGui::GetFrameHeightWithSpacing()*4
    // Default mode: guiGizmo::mode3Axes|guiGizmo::cubeAtOrigin -> 3 Axes with cube @ origin
```

For a directional arrow:
```cpp
    // I assume, for a vec3, a direction starting from origin, 
    // so if you use a vec3 to identify a light spot
    // need to change direction toward origin
    glm::vec3 light(-getLight()));
    if(ImGui::gizmo3D("##Dir1", light), /*, size,  mode */)  setLight(-light);
```

Instead for a 3 axes widget and light spot:
```cpp
    glm::quat qt = getRotation();
    glm::vec3 light(-getLight()));
    if(ImGui::gizmo3D("##gizmo1", qt, light /*, size,  mode */))  { 
        setLight(-light);
        setRotation(qt);
    }
```

And in your render function (or where you prefer) you can get the transformations matrix

```cpp
    glm::mat4 modelMatrix = glm::mat4_cast(qRot);
    // now you have modelMatrix with rotation then can build MV and MVP matrix
```


Look in *imGuIZMO.h*, *imGuIZMO.cpp* and example source code *uiMainDlg.cpp*, for more personalizations: they are well commented.

These are all possible widget calls:
```cpp
IMGUI_API bool gizmo3D(const char*, glm::quat&, float=IMGUIZMO_DEF_SIZE, const int=imguiGizmo::mode3Axes|imguiGizmo::cubeAtOrigin);
IMGUI_API bool gizmo3D(const char*, glm::vec4&, float=IMGUIZMO_DEF_SIZE, const int=imguiGizmo::mode3Axes|imguiGizmo::cubeAtOrigin);
IMGUI_API bool gizmo3D(const char*, glm::vec3&, float=IMGUIZMO_DEF_SIZE, const int=imguiGizmo::modeDirection);

IMGUI_API bool gizmo3D(const char*, glm::quat&, glm::quat&, float=IMGUIZMO_DEF_SIZE, const int=imguiGizmo::modeDual|imguiGizmo::cubeAtOrigin);
IMGUI_API bool gizmo3D(const char*, glm::quat&, glm::vec4&, float=IMGUIZMO_DEF_SIZE, const int=imguiGizmo::modeDual|imguiGizmo::cubeAtOrigin);
IMGUI_API bool gizmo3D(const char*, glm::quat&, glm::vec3&, float=IMGUIZMO_DEF_SIZE, const int=imguiGizmo::modeDual|imguiGizmo::cubeAtOrigin);
```

### Building Example

The example shown in the screenshot is provided.
To build it you can use CMake (3.10 or higher) or the Visual Studio solution project (for VS 2017) in Windows.
You need to have installed [**GLFW**](https://www.glfw.org/) and [**glm**](https://github.com/g-truc/glm) in your compiler search path (LIB/INCLUDE). and obviously [**ImGui**](https://github.com/ocornut/imgui) (a copy is attached, and alredy included in the project/CMakeList.txt)

The CMake file is able to build also an [**EMSCRIPTEN**](https://kripken.github.io/emscripten-site/index.html) version, obviously you need to have installed EMSCRIPTEN SDK on your computer (1.38.10 or higher): look at or use the helper batch/script files, in main example folder, to pass appropriate defines/patameters to CMake command line.

To build the EMSCRIPTEN version, in Windows, with CMake, need to have **mingw32-make.exe** in your computer and search PATH (only the make utility is enough): it is a condition of EMSDK tool to build with CMake in Windows.

