#------------------------------------------------------------------------------
#  Copyright (c) 2018-2024 Michele Morrone
#  All rights reserved.
#
#  https://michelemorrone.eu - https://brutpitt.com
#
#  X: https://x.com/BrutPitt - GitHub: https://github.com/BrutPitt
#
#  direct mail: brutpitt(at)gmail.com - me(at)michelemorrone.eu
#
#  This software is distributed under the terms of the BSD 2-Clause license
#------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10)


# To build a WebGL (EMSCRIPTEN) version type:
#       cmake -DBUILD_EMSCRIPTEN:BOOL=TRUE
# obviously is need to have emsdk installed
set(BUILD_EMSCRIPTEN FALSE CACHE BOOL FALSE)

# To use SDL2 as backend (instead of GLFW) type:
#       cmake -DUSE_SDL:BOOL=TRUE
# obviously is need to have SDL2 (+devel package) installed
set(USE_SDL FALSE CACHE BOOL FALSE)

# To build with VirtualGizmo type:
#       cmake -DUSE_VIRTUALGIZMO:BOOL=TRUE
set(USE_VIRTUALGIZMO FALSE CACHE BOOL FALSE)

set(IMGUI_DIR imgui)

set(CMAKE_CXX_STANDARD 11)

set(M_GLOBAL_FLAGS "-DIMGUI_DIR -DGLM_ENABLE_EXPERIMENTAL")

if(USE_VIRTUALGIZMO)
    message("VirtualGizmo ... ")
    set(M_GLOBAL_FLAGS "${M_GLOBAL_FLAGS} -DGLAPP_USE_VIRTUALGIZMO")
endif()

# relative source directory
set(SRC "${CMAKE_SOURCE_DIR}/src")

set(SOURCE_FILES
    ${SRC}/glApp.h
    ${SRC}/glApp.cpp
    ${SRC}/glWindow.h
    ${SRC}/glWindow.cpp
    ${SRC}/qJulia.h
    ${SRC}/qJulia.cpp
    ${SRC}/ui/uiMainDlg.h
    ${SRC}/ui/uiMainDlg.cpp
    ${SRC}/ui/uiSettings.cpp
    ${SRC}/tools/glslProgramObject.cpp
    ${SRC}/tools/glslShaderObject.cpp
    ${SRC}/tools/glslProgramObject.h
    ${SRC}/tools/glslShaderObject.h    
    ${SRC}/tools/imGuIZMOquat.cpp
    ${SRC}/tools/imGuIZMOquat.h
    ${SRC}/tools/vGizmo.h
    ${SRC}/tools/vgMath.h
    ${SRC}/tools/vgConfig.h
    ${SRC}/libs/${IMGUI_DIR}/imconfig.h
    ${SRC}/libs/${IMGUI_DIR}/imgui.h
    ${SRC}/libs/${IMGUI_DIR}/imgui.cpp
    ${SRC}/libs/${IMGUI_DIR}/imgui_widgets.cpp
    ${SRC}/libs/${IMGUI_DIR}/imgui_tables.cpp
    ${SRC}/libs/${IMGUI_DIR}/imgui_draw.cpp
    ${SRC}/libs/${IMGUI_DIR}/imgui_demo.cpp
    ${SRC}/libs/${IMGUI_DIR}/imgui_impl_opengl3.cpp
    ${SRC}/libs/${IMGUI_DIR}/imgui_impl_opengl3.h
    ${SRC}/libs/${IMGUI_DIR}/imgui_internal.h
    ${SRC}/libs/${IMGUI_DIR}/imstb_rectpack.h
    ${SRC}/libs/${IMGUI_DIR}/imstb_textedit.h
    ${SRC}/libs/${IMGUI_DIR}/imstb_truetype.h)

# additional INCLUDE directories
include_directories(${SRC})
include_directories(${SRC}/libs)
include_directories(${SRC}/tools)
include_directories(${SRC}/../../Shaders) #CLion: add folder to Project tool window
#include_directories($ENV{INCLUDE})
#include_directories("D:/Lang/mingw64/include")

# additional LIB directories
# link_directories($ENV{LIB})


if (${BUILD_EMSCRIPTEN})
    project(wgl_qjSet)

    add_executable(${PROJECT_NAME} ${SOURCE_FILES}
        ${SRC}/libs/${IMGUI_DIR}/imgui_impl_glfw.cpp
        ${SRC}/libs/${IMGUI_DIR}/imgui_impl_glfw.h)

    set(M_GLOBAL_FLAGS "${M_GLOBAL_FLAGS} -DGLAPP_NO_GLSL_PIPELINE -DIMGUI_IMPL_OPENGL_ES3 -DGLFW_INCLUDE_NONE -DGLAPP_NO_OGL_DSA -DGLFW_INCLUDE_ES3")
#    set(CMAKE_C_COMPILER "emcc")
#    set(CMAKE_CXX_COMPILER "emcc")

# -o ${PROJECT_NAME}.html ->  CMAKE_EXECUTABLE_SUFFIX  .html .bc, .js (default), etc
    set(CMAKE_EXECUTABLE_SUFFIX ".html")

# relative emscripten output files
#    set(EMSOUT "emsOut")
#    file(MAKE_DIRECTORY ${EMSOUT})
#    file(MAKE_DIRECTORY ${EMSOUT}/${CMAKE_BUILD_TYPE})
#    set(EMSOUT ${EMSOUT}/${CMAKE_BUILD_TYPE})

# emcc compiler options
    set(M_EMCC_FLAGS "-DGLAPP_NO_OGL_DSA -DGLFW_INCLUDE_ES3")

# BUILD TYPE: [Debug|RelWithDebInfo|Release|MinSizeRel]    
    if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
        set(M_EMCC_BINARYEN "-s WASM=0 -s DEMANGLE_SUPPORT=1 ")
        set(M_EMCC_SHELL_FILE "--shell-file \"${CMAKE_SOURCE_DIR}/shell_minimal.html\"")
    else()
        #  -s BINARYEN_METHOD='native-wasm,asmjs' 'interpret-asm2wasm' -s BINARYEN_ASYNC_COMPILATION=0
        if(${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo")
            set(M_EMCC_BINARYEN "-s WASM=1")
            set(M_EMCC_SHELL_FILE "--shell-file \"${SRC}/shell_minimal.html\"")
        else()
            if(${CMAKE_BUILD_TYPE} MATCHES "MinSizeRel")
                # one file no memory option: -s SINGLE_FILE=1 --memory-init-file 0 
                set(M_EMCC_BINARYEN "-s WASM=1 -s SINGLE_FILE=1")
                set(M_EMCC_SHELL_FILE "--shell-file \"${SRC}/mVeryMinimal.html\"")
            else() # Release 
                # -s SINGLE_FILE=1 --memory-init-file 0 -s \"BINARYEN_METHOD='interpret-asm2wasm'\"
                set(M_EMCC_BINARYEN "-s WASM=1 -s \"BINARYEN_METHOD='native-wasm'\" ")
                set(M_EMCC_SHELL_FILE "--shell-file \"${SRC}/mVeryMinimal.html\"")
            endif()    
        endif()    
    endif()

# grouped EMCC_OPTIONS
    set(M_EMCC_EMBED_FILES "--embed-file \"${CMAKE_SOURCE_DIR}/Shaders/qjFragES2.glsl@Shaders/\" --embed-file \"${CMAKE_SOURCE_DIR}/Shaders/qjVert.glsl@Shaders/\"")
    # set(M_EMCC_EMBED_FILES "--preload-file \"${CMAKE_SOURCE_DIR}/Shaders@/Shaders\"")
    # -s FULL_ES3=1 -s OFFSCREENCANVAS_SUPPORT=1
    set(M_EMCC_OGL_OPTIONS "-s USE_WEBGL2=1 -s USE_GLFW=3 -s LEGACY_GL_EMULATION=0 -s OFFSCREENCANVAS_SUPPORT=1 ")

# merge all EMCC_OPTIONS
#    set(M_EMCC_OPTIONS "-s \"BINARYEN_TRAP_MODE='clamp'\" ${M_EMCC_SHELL_FILE} ${M_EMCC_OGL_OPTIONS} ${M_EMCC_BINARYEN} ${M_EMCC_EMBED_FILES}")
    set(M_EMCC_OPTIONS "-s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=1 -s ALLOW_MEMORY_GROWTH=1 -s PRECISE_F32=1 ${M_EMCC_SHELL_FILE} ${M_EMCC_OGL_OPTIONS} ${M_EMCC_BINARYEN} ${M_EMCC_EMBED_FILES}")

#    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${EMSOUT}")
#    set(CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/${EMSOUT}")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${M_GLOBAL_FLAGS} ${M_EMCC_FLAGS} ${M_EMCC_OPTIONS}")


#    set_target_properties(${PROJECT_NAME}  PROPERTIES LINK_FLAGS "${M_EMCC_OPTIONS}")
else()
    project(qjSet)

    find_package(OpenGL REQUIRED)

    set(M_GLOBAL_FLAGS "${M_GLOBAL_FLAGS} -DIMGUI_IMPL_OPENGL_LOADER_GLAD -DGLFW_INCLUDE_NONE -DGLAPP_NO_OGL_DSA")
    if(USE_SDL)
        add_executable(${PROJECT_NAME} ${SOURCE_FILES} 
            ${SRC}/libs/glad/glad.cpp
            ${SRC}/libs/${IMGUI_DIR}/imgui_impl_sdl.cpp
            ${SRC}/libs/${IMGUI_DIR}/imgui_impl_sdl.h)
        set(M_GLOBAL_FLAGS "${M_GLOBAL_FLAGS} -DGLAPP_USE_SDL")
    else()    
        add_executable(${PROJECT_NAME} ${SOURCE_FILES} 
            ${SRC}/libs/glad/glad.cpp
            ${SRC}/libs/${IMGUI_DIR}/imgui_impl_glfw.cpp
            ${SRC}/libs/${IMGUI_DIR}/imgui_impl_glfw.h)
    endif(USE_SDL)    
    
    set(M_CXX_FLAGS "-DAPP_REQUIRE_OGL45")

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${M_GLOBAL_FLAGS} ${M_CXX_FLAGS}")


    if(WIN32)
        set(TARGET_LIBS "")
    else()
        set(TARGET_LIBS ${CMAKE_DL_LIBS})
    endif()

    if(USE_SDL)
        find_package(SDL2 REQUIRED)
        include_directories(${SDL2_INCLUDE_DIRS})
        target_link_libraries(${PROJECT_NAME} ${OPENGL_gl_LIBRARY} ${SDL2_LIBRARIES} ${TARGET_LIBS})
    else()
        list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")
        if(WIN32)
            find_package(glfw3 REQUIRED)
            #link_directories(${SRC}/libs/glfw/buildWin/lib/${CMAKE_BUILD_TYPE})
            #set(TARGET_LIBS ${CMAKE_DL_LIBS} "${SRC}/libs/glfw/buildWin/lib/${CMAKE_BUILD_TYPE}/glfw3.lib")
            #set(TARGET_LIBS ${CMAKE_DL_LIBS} "-lglfw3.lib")
        else()
            #find_package(glfw3 REQUIRED)
            set(TARGET_LIBS ${CMAKE_DL_LIBS} "-lglfw -lpthread -lX11")
        endif(WIN32)
        target_link_libraries(${PROJECT_NAME} ${TARGET_LIBS} ${OPENGL_gl_LIBRARY})
    endif()

endif ()
