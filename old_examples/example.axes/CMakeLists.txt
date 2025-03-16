#------------------------------------------------------------------------------
#  Copyright (c) 2018-2024 Michele Morrone
#  All rights reserved.
#
#  https://michelemorrone.eu - https://glchaosp.com - https://brutpitt.com
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


set(CMAKE_CXX_STANDARD 11)

set(IMGUI_DIR imgui)



set(M_GLOBAL_FLAGS "-D_USE_GLFW_ -DGLM_ENABLE_EXPERIMENTAL -fpermissive")
if(USE_VIRTUALGIZMO)
    message("VirtualGizmo ... ")
    set(M_GLOBAL_FLAGS "${M_GLOBAL_FLAGS} -DGLAPP_USE_VIRTUALGIZMO")
endif()

# relative source directory
set(SRC "${CMAKE_SOURCE_DIR}/src")
message(${SRC})

set(SOURCE_FILES
    ${SRC}/glApp.cpp
    ${SRC}/glApp.h
    ${SRC}/glWindow.cpp
    ${SRC}/glWindow.h
    ${SRC}/ui/uiMainDlg.h
    ${SRC}/ui/uiMainDlg.cpp
    ${SRC}/ui/uiSettings.cpp
    ${SRC}/tools.old/glslProgramObject.cpp
    ${SRC}/tools.old/glslShaderObject.cpp
    ${SRC}/tools.old/glslProgramObject.h
    ${SRC}/tools.old/glslShaderObject.h
    ${SRC}/tools/imGuIZMOquat.cpp
    ${SRC}/tools/imGuIZMOquat.h
    ${SRC}/tools/vgMath.h
    ${SRC}/tools/vgConfig.h
    ${SRC}/tools/vGizmo.h
    ${SRC}/tools.old/transforms.h
    ${SRC}/tools.old/oglAxes.cpp
    ${SRC}/tools.old/oglAxes.h
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


# additional LIB directories
# link_directories()



if (${BUILD_EMSCRIPTEN})
#    set(CMAKE_C_COMPILER "emcc")
#    set(CMAKE_CXX_COMPILER "emcc")
    project(wglGizmo)

    add_executable(${PROJECT_NAME} ${SOURCE_FILES}
        ${SRC}/libs/${IMGUI_DIR}/imgui_impl_glfw.cpp
        ${SRC}/libs/${IMGUI_DIR}/imgui_impl_glfw.h)


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
        set(M_EMCC_BINARYEN "-s WASM=0 -s \"BINARYEN_METHOD='interpret-s-expr'\"")
        set(M_EMCC_SHELL_FILE "")
    else()
        #  -s BINARYEN_METHOD='native-wasm,asmjs' 'interpret-asm2wasm' -s BINARYEN_ASYNC_COMPILATION=0
        if(${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo")
            set(M_EMCC_BINARYEN "-s WASM=1")
            set(M_EMCC_SHELL_FILE "--shell-file \"${SRC}/shell_minimal.html\"")
        else()
            if(${CMAKE_BUILD_TYPE} MATCHES "MinSizeRel")
                # one file no memory option: -s SINGLE_FILE=1 --memory-init-file 0 
                set(M_EMCC_BINARYEN "-s WASM=1  -s SINGLE_FILE=1 -s \"BINARYEN_METHOD='native-wasm'\" ")
                set(M_EMCC_SHELL_FILE "--shell-file \"${SRC}/mVeryMinimal.html\"")
            else() # Release 
                # -s SINGLE_FILE=1 --memory-init-file 0 -s \"BINARYEN_METHOD='interpret-asm2wasm'\"
                set(M_EMCC_BINARYEN "-s WASM=1 -s \"BINARYEN_METHOD='native-wasm'\" ")
                set(M_EMCC_SHELL_FILE "--shell-file \"${SRC}/mVeryMinimal.html\"")
            endif()    
        endif()    
    endif()

# grouped EMCC_OPTIONS
    set(M_EMCC_EMBED_FILES "--embed-file \"${CMAKE_SOURCE_DIR}/Shaders/oglAxesFrag.glsl@Shaders/\" --embed-file \"${CMAKE_SOURCE_DIR}/Shaders/oglAxesVert.glsl@Shaders/\"")
    # set(M_EMCC_EMBED_FILES "--preload-file \"${CMAKE_SOURCE_DIR}/Shaders@/Shaders\"")
    # -s FULL_ES3=1 -s OFFSCREENCANVAS_SUPPORT=1
    set(M_EMCC_OGL_OPTIONS "-s USE_WEBGL2=1 -s USE_GLFW=3 -s LEGACY_GL_EMULATION=0")

# merge all EMCC_OPTIONS
    #set(M_EMCC_OPTIONS "-s \"BINARYEN_TRAP_MODE='clamp'\" ${M_EMCC_SHELL_FILE} ${M_EMCC_OGL_OPTIONS} ${M_EMCC_BINARYEN} ${M_EMCC_EMBED_FILES}")
    set(M_EMCC_OPTIONS "-s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=1 -s ALLOW_MEMORY_GROWTH=1 -s PRECISE_F32=1 ${M_EMCC_SHELL_FILE} ${M_EMCC_OGL_OPTIONS} ${M_EMCC_BINARYEN} ${M_EMCC_EMBED_FILES}")


#    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${EMSOUT}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${M_GLOBAL_FLAGS} ${M_EMCC_FLAGS} ${M_EMCC_OPTIONS}")

#    set_target_properties(${PROJECT_NAME}  PROPERTIES LINK_FLAGS "${M_EMCC_OPTIONS}")
else()
    project(oglGizmo)

    find_package(OpenGL REQUIRED)

    set(M_GLOBAL_FLAGS "${M_GLOBAL_FLAGS} -DIMGUI_IMPL_OPENGL_LOADER_GLAD -DGLFW_INCLUDE_NONE -DGLAPP_NO_OGL_DSA")
    if(USE_SDL)
        message("SDL!!!!!")
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

#   find_package(glfw3 REQUIRED)

    # add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${SRC}/libs/glad/glad.cpp)

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
        if(WIN32)
            find_package(glfw3 REQUIRED)
        else()    
            #find_package(glfw3 REQUIRED)
        endif()
        target_link_libraries(${PROJECT_NAME} ${OPENGL_gl_LIBRARY} ${TARGET_LIBS} glfw)
    endif()

endif ()
