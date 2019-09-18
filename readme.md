# imGuIZMO.quat
**imGuIZMO.quat** is a [**ImGui**](https://github.com/ocornut/imgui) widget like a trackball: it provides a way to rotate models, lights, or objects with mouse, and graphically visualize their position in space. It uses **quaternions** algebra, internally, to manage rotations, but offers the possibility also to interfacing with **vec3**, **vec4** or **mat4x4** (rotation)

**imGuIZMO** is written in C++ (C++14) and consist of only two files (*imGuIZMO.h* and *imGuIZMO.cpp*) that you can simply add to your project.
It uses [**virtualGizmo3D**](https://github.com/BrutPitt/virtualGizmo3D): my *header only* 3D GIZMO screen manipulator tool (in *Immediate Mode* without pan and dolly functions) and [**glm** mathematics library](https://github.com/g-truc/glm) (0.9.9 or higher), also it an *header only* tool.

### Live WebGL2 demo

You can run/test **WebGL 2** examples of **imGuIZMO** from following links:
- [**imGuIZMO.quat** ImGui widget + 3D (dolly/zoom) screen manipulator](https://brutpitt.github.io/imGuIZMO.quat/imGuIZMO.example/WebGL/qjSetVG.html)
- [**imGuIZMO.quat** ImGui widget manipulator (only)](https://brutpitt.github.io/imGuIZMO.quat/imGuIZMO.example/WebGL/qjSet.html)

<p align="center"><a href="https://brutpitt.github.io/imGuIZMO.quat/imGuIZMO.example/WebGL/qjSetVG.html"> 
<img src="https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/imGuIZMO.gif"></a>
</p>

It works only on browsers with **WebGl 2** and *webassembly* support (FireFox/Opera/Chrome and Chromium based)

Test if your browser supports WebGL 2, here: [WebGL2 Report](http://webglreport.com/?v=2)

****imGuIZMO.quat** was originally developed (still used) for my **[glChAoS.P](https://github.com/BrutPitt/glChAoS.P)** poroject: consult the source code for more examples.*

**[**ImGui**](https://github.com/ocornut/imgui) is Immediate Mode Graphics User Interface library*

<p>&nbsp;<br>&nbsp;<br></p>

## How to use imGuIZMO.quat to manipulate an object with 4 code lines 

**You can manipulate an object with only **4** code lines**

To use imGuIZMO need to include imGuIZMO.h file in your code.
```cpp
#include "imGuIZMO.h"
```
You can think of declaring declare an object of type `glm::quat` (quaternion), global or static or as member of your class, to mantain track of rotations:

```cpp
// For imGuIZMO, declare static or global variable or member class quaternion
    glm::quat qRot = glm::quat(1.f, 0.f, 0.f, 0.f);
```
In your **ImGui** window you call/declare a widget...

```cpp
    ImGui::gizmo3D("##gizmo1", qRot /*, size,  mode */);
```
Finally in your render function (or where you prefer) you can get back the transformations matrix

```cpp
    glm::mat4 modelMatrix = glm::mat4_cast(qRot);
    // now you have modelMatrix with rotation then can build MV and MVP matrix
```
now you have modelMatrix with rotation then can build MV and MVP matrix

### Alternately

Maybe can be more elegant to add two helper functions
```cpp
// two helper functions, not really necessary (but comfortable)
    void setRotation(const glm::quat &q) { qRot = q; }
    glm::quat& getRotation() { return qRot; }
 ```
And to change the widget call

```cpp
    glm::quat qt = getRotation();
    if(ImGui::gizmo3D("##gizmo1", qt /*, size,  mode */)) {  setRotation(qt); }
```
but the essence of the speech does not change


<p>&nbsp;<br>&nbsp;<br></p>

## Widget types

**Axes mode:**
```cpp
    glm::quat qt = getRotation();
    if(ImGui::gizmo3D("##gizmo1", qt /*, size,  mode */)) {  setRotation(qt); }
    // or explicitly
    vec3 dir;
    ImGui::gizmo3D("##Dir1", dir, 100, imguiGizmo::mode3Axes|guiGizmo::cubeAtOrigin);

    // Default size: ImGui::GetFrameHeightWithSpacing()*4
    // Default mode: guiGizmo::mode3Axes|guiGizmo::cubeAtOrigin -> 3 Axes with cube @ origin
```

**Directional arrow:**
```cpp
    // I assume, for a vec3, a direction starting from origin, 
    // so if you use a vec3 to identify a light spot
    // need to change direction toward origin
    glm::vec3 light(-getLight()));
    if(ImGui::gizmo3D("##Dir1", light /*, size,  mode */)  setLight(-light);
    // or explicitly
    if(ImGui::gizmo3D("##Dir1", light, 100, imguiGizmo::modeDirection)  setLight(-light);
```
**Directional plane:**
```cpp
    // I assume, for a vec3, a direction starting from origin, 
    // so if you use a vec3 to identify a light spot
    // need to change direction toward origin
    glm::vec3 dir(1.0, 0.0, 0.0);
    if(ImGui::gizmo3D("##Dir1", dir, 100,  imguiGizmo::modeDirPlane)  { }
```

**Axes and spot:**
```cpp
    glm::quat qt = getRotation();
    glm::vec3 light(-getLight()));
    if(ImGui::gizmo3D("##gizmo1", qt, light /*, size,  mode */))  { 
        setLight(-light);
        setRotation(qt);
    }
```

These are all possible widget calls:
```cpp
IMGUI_API bool gizmo3D(const char*, glm::quat&, float=IMGUIZMO_DEF_SIZE, const int=imguiGizmo::mode3Axes|imguiGizmo::cubeAtOrigin);
IMGUI_API bool gizmo3D(const char*, glm::vec4&, float=IMGUIZMO_DEF_SIZE, const int=imguiGizmo::mode3Axes|imguiGizmo::cubeAtOrigin);
IMGUI_API bool gizmo3D(const char*, glm::vec3&, float=IMGUIZMO_DEF_SIZE, const int=imguiGizmo::modeDirection);

IMGUI_API bool gizmo3D(const char*, glm::quat&, glm::quat&, float=IMGUIZMO_DEF_SIZE, const int=imguiGizmo::modeDual|imguiGizmo::cubeAtOrigin);
IMGUI_API bool gizmo3D(const char*, glm::quat&, glm::vec4&, float=IMGUIZMO_DEF_SIZE, const int=imguiGizmo::modeDual|imguiGizmo::cubeAtOrigin);
IMGUI_API bool gizmo3D(const char*, glm::quat&, glm::vec3&, float=IMGUIZMO_DEF_SIZE, const int=imguiGizmo::modeDual|imguiGizmo::cubeAtOrigin);
```
<p>&nbsp;<br></p>

For for more details, more personalizations, or how to change sizes, color, thickness, etc... examine the attached example source code (`uiMainDlg.cpp` file), or again `imGuIZMO.h`, `imGuIZMO.cpp` files: they are well commented.
The widget are also used in **[glChAoS.P](https://github.com/BrutPitt/glChAoS.P)** poroject.

<p>&nbsp;<br>&nbsp;<br></p>

## All widgets visualization

**FOUR** widget types are prvided, (six function calls with different parameters: *quaternion, vec4, vec3* for different uses) each of them personalizable with several graphics options:

### Axes mode
| ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/A001.jpg) | ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/A002.jpg) | ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/A003.jpg) | ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/A004.jpg) |
| --- | --- | --- | --- |


### Directional arrow
| ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/B001.jpg) | ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/B002.jpg) | ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/B003.jpg) |
| --- | --- | --- |

### Plane direction 
| ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/U0006.jpg) | ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/U0008.jpg) | 
| --- | --- |

### Axes + spot
| ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/U0001.jpg) | ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/U0003.jpg) | ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/C003.jpg) | ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/C004.jpg) |
| --- | --- | --- | --- |


### And much more...
Full configurable: Lenght, thickness, dimensions, number of polygon slices, colors and sphere tesselation:

| ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/D001.jpg) | ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/D002.jpg) | ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/D003.jpg) | ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/U0007.jpg)|
| --- | --- | --- | --- |





<p>&nbsp;<br>&nbsp;<br></p>

### Building Example

The example shown in the screenshot is provided.

In  example I use **GLFW** or **SDL2** (via `#define GLAPP_USE_SDL`) with **OpenGL**, but it is simple to change if you use Vulkan/DirectX/etc, other frameworks (like GLUT) or native OS access.

To use SDL framework instead of GLFW, uncomment `#define GLAPP_USE_SDL` in `glApp.h` file, or pass `-DGLAPP_USE_SDL` directly to compiler.

To build it you can use CMake (3.10 or higher) or the Visual Studio solution project (for VS 2017) in Windows.
You need to have [**GLFW**](https://www.glfw.org/) (or [**SDL**](https://libsdl.org/)) in your compiler search path (LIB/INCLUDE). Instead copy of [**glm**](https://github.com/g-truc/glm) and [**ImGui**](https://github.com/ocornut/imgui) are attached and included in the example.

The CMake file is able to build also an [**EMSCRIPTEN**](https://kripken.github.io/emscripten-site/index.html) version, obviously you need to have installed EMSCRIPTEN SDK on your computer (1.38.10 or higher): look at or use the helper batch/script files, in main example folder, to pass appropriate defines/patameters to CMake command line.

To build the EMSCRIPTEN version, in Windows, with CMake, need to have **mingw32-make.exe** in your computer and search PATH (only the make utility is enough): it is a condition of EMSDK tool to build with CMake in Windows.


**For windows users that use vs2017 project solution:**
The current VisualStudio project solution refers to my environment variable RAMDISK (`R:`), and subsequent VS intrinsic variables to generate binary output:
`$(RAMDISK)\$(MSBuildProjectDirectoryNoRoot)\$(DefaultPlatformToolset)\$(Platform)\$(Configuration)\` 
Even without a RAMDISK variable, executable and binary files are outputted in base to the values of these VS variables, starting from root of current drive.