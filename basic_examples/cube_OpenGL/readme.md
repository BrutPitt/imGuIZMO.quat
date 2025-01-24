# ImGuIZMO_quat OpenGL easy examples

These "step by step" examples guide in the use of the various widgets and and how to use the transformations obtained to build an MVP matrix.
They cover both GLFW and SDL2 frameworks in distinct/separate files.

<p align="center"><a href="https://brutpitt.github.io/myRepos/imGuIZMO/example/WebGL/wglCubeExample.html"> 
<img width="640" height="400" src="https://brutpitt.github.io/myRepos/imGuIZMO/screenshots/cube_ex.png"></a>
</p>

#### Live WebGL2 [glCube_07 - easy example](https://brutpitt.github.io/myRepos/imGuIZMO/example/WebGL/wglCubeExample.html) 



### Build examples

`CMakeLists.txt` file is provided to build the examples

From command (where is `CMakeLists.txt`) line type:
- `mkdir build` ==> create a directory where to build
- `cd build` ==> go to in it   

Then execute CMake depending on your setting (devel packages installed):
- `cmake -G Ninja ..` ==> to build with **ninja** and GLFW framework
- `cmake -G "Unix Makefiles"" ..` ==> to build with **make** and GLFW framework
- `cmake -DUSE_SDL3=ON -G Ninja ..` ==> to build with **ninja** and SDL3 framework

And now type
`ninja` or `make -j` or run the *building system* chosed to build examples


**currently tested in Linux, but it should also work in Windows and/or MacOS... it will be tested soon on both