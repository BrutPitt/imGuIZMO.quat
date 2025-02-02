# ImGuIZMO_quat OpenGL easy examples

These "step by step" examples guide in the use of the various widgets and and how to use the transformations obtained to build an MVP matrix.
They cover both GLFW and SDL2 frameworks in distinct/separate files.

<p align="center"><a href="https://brutpitt.github.io/myRepos/imGuIZMO/example/WebGL/wglCubeExample.html"> 
<img width="640" height="400" src="https://brutpitt.github.io/myRepos/imGuIZMO/screenshots/lightCube.png"></a>
</p>

#### Live WebGL2 [glLightCube_07](https://brutpitt.github.io/myRepos/imGuIZMO/example/WebGL/wglLightCube.html) and [glCube_07](https://brutpitt.github.io/myRepos/imGuIZMO/example/WebGL/wglLightCube.html)
**you can find they in [easy cube examples](https://github.com/BrutPitt/imGuIZMO.quat/tree/master/basic_examples/cube_OpenGL) and [illuminated cube](https://github.com/BrutPitt/imGuIZMO.quat/tree/master/examples/OpenGL/glLightCube)*



### Build examples

`CMakeLists.txt` file is provided to build the examples

From command line (where is `CMakeLists.txt`)  type:
- `mkdir build` ==> create a directory where to build
- `cd build` ==> go to in it   

Then execute CMake depending on your settings (devel packages installed).
For example, use:
- `cmake -G Ninja ..` ==> to build with **ninja** and GLFW framework
- `cmake -G "Unix Makefiles"" ..` ==> to build with **make** and GLFW framework
- `cmake -DUSE_SDL3=ON -G Ninja ..` ==> to build with **ninja** and SDL3 framework

And now run the command:
`ninja` or `make -j` or the *building system* chosed to build examples

**obviously is necessary to have GLFW | SDL2 (devel package) installed: one is enaugh*
**currently tested in Linux, but it should also work in Windows and/or MacOS... it will be tested soon on both*