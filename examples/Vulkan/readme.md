# ImGuIZMO_quat Vulkan easy examples
These *easy* examples was written using `vulkan.hpp` / vulkan-hpp 

<p align="center"><a href="https://brutpitt.github.io/myRepos/imGuIZMO/example/WebGL/wglCubeExample.html"> 
<img width="640" height="400" src="https://brutpitt.github.io/myRepos/imGuIZMO/screenshots/lightCube.png"></a>
</p>

These "step by step" examples guide in the use of the various widgets and and how to use the transformations obtained to build an MVP matrix.
They are similar to OpenGL examples or to LIVE **WebGL2** [glLightCube_07](https://brutpitt.github.io/myRepos/imGuIZMO/example/WebGL/wglLightCube.html) or [glCube_07](https://brutpitt.github.io/myRepos/imGuIZMO/example/WebGL/wglLightCube.html), but using Vulkan engine.
**you can find they in [easy cube examples](https://github.com/BrutPitt/imGuIZMO.quat/tree/master/basic_examples/cube_OpenGL) and [illuminated cube](https://github.com/BrutPitt/imGuIZMO.quat/tree/master/examples/OpenGL/glLightCube)*

In the OpenGL version of the examples was opted to separate the GLFW and SDL versions in distinct files.
If you are starting to become familiar with widgets I recommend starting from the OpenGL examples: they are more linear and differentiated (SDL/GLFW), and using (in a second time) the Vulkan examples to view the interaction with this graphical backend.

In Vulkan, given the greater complexity of the graphical interface and the integration of the framwork with the creation of the Vulkan surface and any example have CMake build options for GLFW / SDL2 / SDL3.
- GLFW is the DEFAULT option
- `cmake -DUSE_SDL2=ON` builds for SDL2
- `cmake -DUSE_SDL3=ON` builds for SDL3
*obviously is necessary to have GLFW | SDL2 | SDL3 (devel package) installed: one is enaugh*

They were encapsulated in a `framwork` base class (and its subclasses: files `framework.cpp` and `framework.h`): these classes manage mouse and other framework events, and also **ImGui** specific framwork interaction (for ImGuIZMOquat) in a univocal way.
This permits more linearity in reading the code (in "main render loop/function") and all the framework calls are well commented to show the specific use.

The `vkCube` code is divided into (main) 2 parts:
- `vkAppBase class` the bulk of Vulkan initialization 
- `vkApp class` draw and manipulate scene objects (and initialization of ImGui)

All widgets/controls and 3D manipulation code (or almost) was relegated in the [`run()`](https://github.com/BrutPitt/imGuIZMO.quat/blob/24799735456308e298f07474eab2eea90d4941ca/basic_examples/cube_Vulkan/vkCube_06.cpp#L404) function (of `vkApp class`, of `vkCube_*.cpp` files). 
(look also [`setScene()`](https://github.com/BrutPitt/imGuIZMO.quat/blob/24799735456308e298f07474eab2eea90d4941ca/basic_examples/cube_Vulkan/vkCube_06.cpp#L332) )


For those who want to use too [**vGizmo3D**](https://github.com/BrutPitt/virtualGizmo3D)
The [initialization of vGizmo3D](https://github.com/BrutPitt/virtualGizmo3D/blob/b9162b8f38ebbe57508cabe37e0238c053355e92/easy_examples/cube_Vulkan/framework.cpp#L42) (*framework agnostic*) is in the framework class
The *mouse control events* and the *key modifier checks* are specific for any framework:
- **GLFW**: check [Key Modifiers](https://github.com/BrutPitt/virtualGizmo3D/blob/b9162b8f38ebbe57508cabe37e0238c053355e92/easy_examples/cube_Vulkan/framework.cpp#L251-L262) - check [Mouse event](https://github.com/BrutPitt/virtualGizmo3D/blob/b9162b8f38ebbe57508cabe37e0238c053355e92/easy_examples/cube_Vulkan/framework.cpp#L266-L292)

- **SDL2/SDL3**: check [Key Modifiers](https://github.com/BrutPitt/virtualGizmo3D/blob/b9162b8f38ebbe57508cabe37e0238c053355e92/easy_examples/cube_Vulkan/framework.cpp#L127-L141) - check [Mouse event](https://github.com/BrutPitt/virtualGizmo3D/blob/b9162b8f38ebbe57508cabe37e0238c053355e92/easy_examples/cube_Vulkan/framework.cpp#L145-L174)



## Different representation of the 3D space
Despite a different representation of the 3D space between Vulkan and OpenGL/Webgl, there are no changes in the use of **imgiuzmo_quat / ImGuIZMO.quat** or [**vGizmo3D**](https://github.com/BrutPitt/virtualGizmo3D).
This not also thanks to ImGui (or GLFW/SDL for vGizmo3D) that report same mouse coords on widget ([0,0] is in top-left corner), but mainly thanks to the choice to select a "clip matrix" consonant to my needs to build MVP Matrix.

In `vkCube_*` examples I have used a *clip-Matrix* with (only) `Y` inverted and "half" `Z`.

If you have difficulty to understand my transforms, I invite to read: [Setting Up a Proper Projection Matrix for Vulkan](https://johannesugb.github.io/gpu-programming/setting-up-a-proper-vulkan-projection-matrix/), by Johannes Unterguggenberger (not same, but very similar)
Are also mentioned other tecniques to maintain compatibility on grahical backends (OpenGL/Vulkan), also the [Flipping the Vulkan viewport](https://www.saschawillems.de/blog/2019/03/29/flipping-the-vulkan-viewport/) used by Sascha Willems in some of your "very famous" [Vulkan C++ examples and demos](https://github.com/SaschaWillems/Vulkan)

Using the *clip-Matrix* have allowed me to have a rapresentation of 3D space similar to "Cartesian" 3D space (Y grows up) also in Vulkan.
But, since everyone can do what they want with their own "model" in the **ImGuIZMO.quat** / [**vGizmo3D**](https://github.com/BrutPitt/virtualGizmo3D) v4.0 I added the possibility:
- To reverse: X/Y rotation, X/Y Pan & Dolly movements (code setting or defaults: via `imguizmo_config.h` or compile defines)
- To reverse (toward/direction) the widget axes, grafically, to adapt to your 3D space (only imGuIZMOquat, of course)

In this example/s I added some other models of 3d space "transformation":
e.g. full inversion `-Y` and `-Z` axes, or "untouched" Vulkan space (Y grows down, and Z forward), without *clip-Matrix*, to show how to use/set flipping rotation on controls.
In the end of `vkCube.h` you can find these possibility (un-comment what you prefer)


## Build examples

`CMakeLists.txt` file is provided to build the examples

From command line (where is `CMakeLists.txt`) type:
- `mkdir build` ==> create a directory where to build
- `cd build` ==> go to in it   

Then execute CMake depending on your settings (devel packages installed).
For example, use:
- `cmake -G Ninja ..` ==> to build with **ninja** and GLFW framework
- `cmake -G "Unix Makefiles"" ..` ==> to build with **make** and GLFW framework
- `cmake -DUSE_SDL3=ON -G Ninja ..` ==> to build with **ninja** and SDL3 framework

And now run the command:
`ninja` or `make -j` or run the *building system* chosed to build examples

**obviously is necessary to have GLFW | SDL2 | SDL3 (devel package) installed: one is enaugh*
**currently tested in Linux, but it should also work in Windows and/or MacOS... it will be tested soon on both*
