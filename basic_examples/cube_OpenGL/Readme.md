# [**glslMandel**](http://www.michelemorrone.eu/WebGL/glslMandel/Mandel.html)

Explore Mandelbrot set in OpenGL 

To build need to have GLFW library (binary and devel)

**Files:**
- `Mandelf.cpp` -> explore mandelbrot with single (float) precision
- `Mandeld.cpp` -> explore mandelbrot with double precision
- `Mandeld_fullColor.cpp` -> explore mandelbrot with double precision and logaritmic smooth coloring

Files, all files, have extension `.cpp` but are written in `C`

To build use:

`gcc -I. -lglfw -ld -lGL Mandel[d|f].cpp glad/glad.cpp`

or

`clang -I. -lglfw -ld -lGL Mandel[d|f].cpp glad/glad.cpp`

For **OS X** users, need to add `-D_OPENGL_4_1_` flag (OS X do not supports OpenGL 4.5)

**Windows** users can use the VisualStudio project attached.

Live/online **WebGL 2** & **WebAssembly** versions available

Simple version: https://michelemorrone.eu/WebGL/glslMandel/Mandel.html

More complex with logarithmic smooth coloring: https://michelemorrone.eu/WebGL/glslMandelColor/Mandel.html


|![](https://raw.githubusercontent.com/BrutPitt/glslMandel/master/screenShots/Mandel.jpg)|![](https://raw.githubusercontent.com/BrutPitt/glslMandel/master/screenShots/Mandel2.jpg)|
| --- | --- |

To run need a browser that have **WebGL2** & **WebAssembly** features.
Currently only FireFox or Chromium based browser (Chrome, Opera, Vivaldi, etc) supports it, in case of problems check for latest version or download a portable version.
You can test the WebGL capabilities of your browser at the following link: [WebGL 2 Report](http://webglreport.com/?v=2)

