# imGuIZMO.quat
**imGuIZMO.quat** is a [**ImGui**](https://github.com/ocornut/imgui) widget: like a trackball it provides a way to rotate models, lights, or objects with mouse, and graphically visualize their position in space, also around any single axis (*Shift/Ctrl/Alt/Super*). It uses **quaternions** algebra, internally, to manage rotations, but offers the possibility also to interfacing with **vec3**, **vec4** or **mat4x4** (rotation)

With **imGuIZMO.quat** you can manipulate an object **with only 4 code lines!** &nbsp; &nbsp; *(read below)*

**imGuIZMO.quat** is written in C++ (C++14) and consist of only two files (*imGuIZMO.h* and *imGuIZMO.cpp*) that you can simply add to your project.
It uses [**virtualGizmo3D**](https://github.com/BrutPitt/virtualGizmo3D): my *header only* 3D GIZMO screen manipulator tool (in *Immediate Mode* without pan and dolly functions) and [**glm** mathematics library](https://github.com/g-truc/glm) (0.9.9 or higher), also it an *header only* tool.

****imGuIZMO.quat** is widget/control for [**ImGui**](https://github.com/ocornut/imgui) Immediate Mode GUI library*

### Live WebGL2 demo

You can run/test **WebGL 2** examples of **imGuIZMO** from following links:
- [**imGuIZMO.quat** ImGui widget + 3D (dolly/zoom) screen manipulator](https://brutpitt.github.io/imGuIZMO.quat/example/WebGL/qjSetVG.html)
- [**imGuIZMO.quat** ImGui widget manipulator (only)](https://brutpitt.github.io/imGuIZMO.quat/example/WebGL/qjSet.html)

<p align="center"><a href="https://brutpitt.github.io/imGuIZMO.quat/example/WebGL/qjSetVG.html"> 
<img src="https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/imGuIZMO.gif"></a>
</p>

It works only on browsers with **WebGl 2** and *webassembly* support (FireFox/Opera/Chrome and Chromium based): test if your browser supports **WebGL2**, here: [WebGL2 Report](http://webglreport.com/?v=2)

****imGuIZMO.quat** was originally developed (still used) for my **[glChAoS.P](https://github.com/BrutPitt/glChAoS.P)** poroject: consult the source code for more examples.*


<p>&nbsp;<br></p>

## How to use [imGuIZMO.quat](https://brutpitt.github.io/imGuIZMO.quat) to manipulate an object with 4 code lines 

To use **imGuIZMO.quat** need to include `imGuIZMO.h` file in your code.
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

### alternately

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
but the essence of the code does not change


<p>&nbsp;<br>&nbsp;<br></p>

## Widget types

**Axes mode:**
```cpp
    glm::quat qt = getRotation();
// get/setRotation are helper funcs that you have ideally defined to manage your global/member objs
    if(ImGui::gizmo3D("##gizmo1", qt /*, size,  mode */)) {  setRotation(qt); }
    // or explicitly
    static vec4 dir;
    ImGui::gizmo3D("##Dir1", dir, 100, imguiGizmo::mode3Axes|guiGizmo::cubeAtOrigin);

    // Default size: ImGui::GetFrameHeightWithSpacing()*4
    // Default mode: guiGizmo::mode3Axes|guiGizmo::cubeAtOrigin -> 3 Axes with cube @ origin
```

**Directional arrow:**
```cpp
// I assume, for a vec3, a direction starting from origin, so if you use a vec3 to identify a light spot
// need to change direction toward origin
    glm::vec3 light(-getLight()));
// get/setLigth are helper funcs that you have ideally defined to manage your global/member objs
    if(ImGui::gizmo3D("##Dir1", light /*, size,  mode */)  setLight(-light);
    // or explicitly
    if(ImGui::gizmo3D("##Dir1", light, 100, imguiGizmo::modeDirection)  setLight(-light);

    // Default arrow color is YELLOW: ImVec4(1.0, 1.0, 0.0, 1.0);
```
**Directional plane:**
```cpp
    static glm::vec3 dir(1.0, 0.0, 0.0);
    if(ImGui::gizmo3D("##Dir1", dir, 100,  imguiGizmo::modeDirPlane)  { }

    // Default direction color is same of default arrow color: YELLOW -> ImVec4(1.0, 1.0, 0.0, 1.0);
    // Default plane color is: ImVec4(0.0f, 0.5f, 1.0f, STARTING_ALPHA_PLANE);
```

**Axes and spot:**
```cpp
// I assume, for a vec3, a direction starting from origin, so if you use a vec3 to identify a light spot
// need to change direction toward origin, it's maintained for uniformity even in spot
    glm::vec3 light(-getLight()));
    glm::quat qt = getRotation();
// get/setLigth get/setRotation are helper funcs that you have ideally defined to manage your global/member objs
    if(ImGui::gizmo3D("##gizmo1", qt, light /*, size,  mode */))  { 
        setLight(-light); 
        setRotation(qt);
    }
    // Default size: ImGui::GetFrameHeightWithSpacing()*4
    // Default mode: guiGizmo::mode3Axes|guiGizmo::cubeAtOrigin -> 3 Axes with cube @ origin
    // Default spot color is same of default arrow color: YELLOW -> ImVec4(1.0, 1.0, 0.0, 1.0);
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
<p> &nbsp; </p>

For for more details, more personalizations, or how to change sizes, color, thickness, etc... examine the attached example source code (`uiMainDlg.cpp` file), or again `imGuIZMO.h`, `imGuIZMO.cpp` files: they are well commented.
The widget are also used in **[glChAoS.P](https://github.com/BrutPitt/glChAoS.P)** poroject.

**If you want use (also) full-screen manipulator, outside **ImGui** widget, look at [**virtualGizmo3D**](https://github.com/BrutPitt/virtualGizmo3D) (is its feature)... also in attached example, enabling `#define GLAPP_USE_VIRTUALGIZMO` define in `glWindow.cpp` file*


**Sizes and colors**

To change size and color of one or all widgets, **imGuIZMO.quat** have some [helper funcs](https://github.com/BrutPitt/imGuIZMO.quat/blob/master/imGuIZMO.quat/imGuIZMO.h#L115#L145)

Just an example...

To change the default color for all ARROW-Direction widgets call once (maybe in your **ImGui** style-settings func):
```cpp    
    imguiGizmo::setDirectionColor(ImVec4(0.5, 1.0, 0.3, 1.0)); // change the default ArrowDirection color
```
Instead to change the color of a single widget:
```cpp    
    imguiGizmo::setDirectionColor(ImVec4(0.5, 1.0, 0.3, 1.0)); // change ArrowDirection color
    ImGui::gizmo3D("##Dir1", dir);                             // display widget with changed color
    imguiGizmo::restoreDirectionColor();                       // restore old ArrowDirection color
```
It's like the push/pop mechanism used in **ImGui**, but only that I don't have a stak (for now I don't see the reason): just a single variable where to save the value. The other functions work in the same way.



<p>&nbsp;<br>&nbsp;<br></p>


## All widgets visualization

**FOUR** widget types are prvided, (six function calls with different parameters: *quaternion, vec4, vec3* for different uses) each of them personalizable with several graphics options:

### Axes mode
| ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/A001.jpg) | ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/A002.jpg) | ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/A003.jpg) | ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/A004.jpg) |
| :---: | :---: | :---: | :---: |


### Directional arrow
| ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/B001.jpg) | ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/B002.jpg) | ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/U0009.jpg) | ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/B003.jpg) |
| :---: | :---: | :---: | :---: | 

### Plane direction 
| ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/U0006.jpg) | ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/U0008.jpg) | ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/U0010.jpg) | ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/U0011.jpg) | 
| :---: | :---: | :---: | :---: |

### Axes + spot
| ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/U0003.jpg) | ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/C004.jpg) | ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/U0001.jpg) | ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/C003.jpg) |
| :---: | :---: | :---: | :---: |


### And much more...
Full configurable: Lenght, thickness, dimensions, number of polygon slices, colors and sphere tesselation:

| ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/D002.jpg) | ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/U0005.jpg) | ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/D001.jpg) | ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/U0007.jpg)| ![alt text](https://raw.githubusercontent.com/BrutPitt/imGuIZMO/master/screenshots/D003.jpg) |
| :---: | :---: | :---: | :---: | :---: |



<p>&nbsp;<br>&nbsp;<br></p>

### Building Example

The source code example shown in the animated gif screenshot, is provided.

In  example I use **GLFW** or **SDL2** (via `#define GLAPP_USE_SDL`) with **OpenGL**, but it is simple to change if you use Vulkan/DirectX/etc, other frameworks (like GLUT) or native OS access.

To build it you can use CMake (3.10 or higher) or the Visual Studio solution project (for VS 2017) in Windows.
You need to have [**GLFW**](https://www.glfw.org/) (or [**SDL**](https://libsdl.org/)) in your compiler search path (LIB/INCLUDE). Instead copy of [**glm**](https://github.com/g-truc/glm) and [**ImGui**](https://github.com/ocornut/imgui) are attached and included in the example.

**CMake**

Use the following command-line defines to enable different options:  
  - `-DUSE_SDL:BOOL=TRUE` to enable **SDL** framwork instead of **GLFW**
  - `-DUSE_VIRTUALGIZMO:BOOL=TRUE` to use also (together) [**virtualGizmo3D**](https://github.com/BrutPitt/virtualGizmo3D) to manipulate objects

**this flags are aviables also in CMakeGUI*

To build [**EMSCRIPTEN**](https://kripken.github.io/emscripten-site/index.html) example, use batch/script files:

* `emsCMakeGen.cmd %EMSCRIPTEN% %BUILD_TYPE%` for **Windows** users
* `sh emsCMakeGen.sh %EMSCRIPTEN% %BUILD_TYPE%` for **Linux** or **OS/X** users

where:
- `%EMSCRIPTEN%` is your emscripten installation path (e.g. `C:\emsdk\emscripten\1.38.10`)
- `%BUILD_TYPE%` is build type: `Debug | Release | RelWithDebInfo | MinSizeRel` 

They are located in root example directory, or examine their content to pass appropriate defines/patameters to CMake command line.

**To build with [**EMSCRIPTEN**](https://kripken.github.io/emscripten-site/index.html), obviously you need to have installed EMSCRIPTEN SDK on your computer (1.38.10 or higher)*

**Emscripten in Windows**

To build the **EMSCRIPTEN** version, in Windows, with CMake, need to have **mingw32-make.exe** in your computer and search PATH (only the make utility is enough): it is a condition of EMSDK tool to build with CMake in Windows.

**VS2017 project solution**
* To build **SDL** or **GLFW**, select appropriate build configuration
* If you have **GLFW** and/or **SDL** headers and library directory paths added to `INCLUDE` and `LIB` environment vars, the compiler find them.
* If you want use (also) full-screen manipulator [**virtualGizmo3D**](https://github.com/BrutPitt/virtualGizmo3D) together with **imGuIZMO.quat**, enable `#define GLAPP_USE_VIRTUALGIZMO` define in `glWindow.cpp` file.
* The current VisualStudio project solution refers to my environment variable RAMDISK (`R:`), and subsequent VS intrinsic variables to generate binary output:
`$(RAMDISK)\$(MSBuildProjectDirectoryNoRoot)\$(DefaultPlatformToolset)\$(Platform)\$(Configuration)\`, so without a RAMDISK variable, executable and binary files are outputted in base to the values of these VS variables, starting from root of current drive. &nbsp;&nbsp; *(you will find built binary here... or change it)*