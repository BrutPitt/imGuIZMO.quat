# vGizmo3D - WGPU / WebGPU example

This is a bit more complex example (respect to [basic_examples](https://github.com/BrutPitt/imGuIZMO.quat/tree/master/basic_examples)) that use **WebGPU** as graphics backend .
It was written in C++ and can be compiled in native mode (for a standalone desktop application) or with EMSCRIPTEN to perform it via the web browser

<p align="center"><a href="https://brutpitt.github.io/myRepos/imGuIZMO/example/WebGL/wglCubeExample.html"> 
<img width="640" height="400" src="https://brutpitt.github.io/myRepos/imGuIZMO/screenshots/lightCube.png"></a>
</p>

### Live WebGPU demo
- **NEW [v4.0 **WebGPU** lighted cube example](https://brutpitt.github.io/myRepos/imGuIZMO/example/WGPU/imguizmo_quat_wgpuLightCube.html)** - from new [examples](https://github.com/BrutPitt/imGuIZMO.quat/tree/master/examples) ( WebGPU / Vulkan / OpenGL )\
also displayed in [full resizable canvas](https://brutpitt.github.io/myRepos/imGuIZMO/example/WGPU/imguizmo_quat_wgpuLightCube_full.html) (w/o instructions and mouse buttons helper)


**It's necessary to use a browser with **WebGPU** capabilities: e.g. Chrome-Canary, FireFox Nightly, Safari Technology Preview ...*



## How to Build                 
This example use Google DAWN (as WGPU SDK) to build native executable (CMakeLists.txt). 


It's necessary to have installed **GLFW** (development package) and python3 (Google DAWN requires)

### Native - Desktop application

- clone Google DAWN (WGPU SDK): `git clone https://dawn.googlesource.com/dawn`
- Install Ninja build system (DAWN requires)
- from/inside `cube_WGPU` folder type: `cmake -B build -DCURRENT_DAWN_DIR=path/where/cloned/dawn` (absolute or relative path) 
- then `cmake --build build`

### Emscripten - Web Browser application (WASM)

- Install Emscripten SDK following the instructions: https://emscripten.org/docs/getting_started/downloads.html
- Install Ninja build system 
- `emcmake cmake -G Ninja -B build`
- `cmake --build build`

then

- `emrun build/name_of_example.html`

or

- `python -m http.server` (in `build` folder)... then open WGPU browser with url: `http://localhost:8000/name_of_example.html`








