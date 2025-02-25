# vGizmo3D Vulkan easy examples
These *easy* examples was written using `vulkan.hpp` / vulkan-hpp      



They are similar to OpenGL examples and to [vGizmo3D v3.1 WebGL2 - easy_cube example](https://brutpitt.github.io/myRepos/vGizmo3D/wglCubeExamples.html)

<p>&nbsp;<br></p>
In the OpenGL version of the examples, it was opted to separate the GLFW and SDL versions in distinct files.

If you are starting to become familiar with widgets I recommend starting from the OpenGL examples: they are more linear and differentiated (SDL/GLFW), and using Vulkan examples to view the interaction with this graphical backend

In Vulkan, given the greater complexity of the graphical interface and the integration of the framwork with the creation of the Vulkan surface, there any example have CMake compiler options for GLFW / SDL2 / SDL3.
- GLFW is the DEFAULT option
- `cmake -DUSE_SDL2=ON` builds for SDL2
- `cmake -DUSE_SDL3=ON` builds for SDL3
*obviously is necessary to have GLFW | SDL2 | SDL3 (devel package) installed: one is enaugh*

They were encapsulated in a `framwork` base class and its subclasses (files `framework.cpp` and `framework.h`): it manages mouse and other framework events, and also **ImGui** specific interaction, in a univocal way.
This permits more linearity in reading the code (of "main render loop/function") and the differences are commented.

The `vkCube` code is divided into (main) 2 parts:
- `vkAppBase class` the bulk of Vulkan initialization 
- `vkApp class` draw and manipulate scene objects (and initialization of ImGui)

But all (almost) was relegated in the `run()` function of `vkApp class`, of `vkCube_*.cpp` files. 
(look also `setScene()`)


## Different representation of the 3D space
Despite a different representation of the 3D space between Vulkan and OpenGL/Webgl, there are no changes in the use of imgiuzmo_quat / ImGuIZMO.quat or vGizmo3D.
This not also thanks to frameworks (ImGui) that report same mouse coords on widget ([0,0] is in top-left corner) always, but mainly thanks to the choice to select a "clip matrix space" consonant to my needs.

Changing the representation of the model is necessary to choose a different Projection Matrix for Vulkan to maintain (respect OpenGL/WebGL).
If you have difficulty I invite to read: [Setting Up a Proper Projection Matrix for Vulkan](https://johannesugb.github.io/gpu-programming/setting-up-a-proper-vulkan-projection-matrix/) 


In `vkCube` example I have used a *clip-Matrix*, just a bit different from previous link, and with (only) `Y` inverted and "half" `Z`.

This permit me to have a rappresentation of 3D space similar to "Cartesian" rappresentation, and no invert the rotation (on some axie) of the wigets/trackball (imguizmo_quat / vGizmo3D).
But, since everyone can do what they want with their own "model" in the ImGuIZMO.quat v4.0 I added the possibility:
- To invert rotations: X/Y rotation, X/Y Pan & Dolly (code setting or defaults: via `imguizmo_config.h` or compile defines)
- To invert (versus/direction) the widget axes, grafically, to adapt to your 3D space

In the examples I added some other models of 3d space "transformation":
Full inversion `-Y` and `-Z`, or "untouched" Vulkan model: in the end of `vkCube` have added this possibility (un-comment what you prefer)




