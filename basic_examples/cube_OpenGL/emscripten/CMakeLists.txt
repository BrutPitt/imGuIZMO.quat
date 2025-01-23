#------------------------------------------------------------------------------
#  Copyright (c) 2018-2025 Michele Morrone
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
cmake_minimum_required(VERSION 3.27)

option(BUILD_EMSCRIPTEN "Build EMSCRIPTEN" ON)

set(CMAKE_CXX_STANDARD 17)

set(SRC          ${CMAKE_SOURCE_DIR}/..)
set(COMMONS_DIR  ${SRC}/../commons)
message(${COMMONS_DIR})
set(TOOLS_DIR    ${SRC}/../../libs)
set(IMGUI_DIR    ${TOOLS_DIR}/imgui)
set(IMGUIZMO_PARENT_DIR ${SRC}/../..)
set(IMGUIZMO_DIR ${IMGUIZMO_PARENT_DIR}/imguizmo_quat)

include_directories(${TOOLS_DIR})
include_directories(${IMGUIZMO_DIR})
include_directories(${COMMONS_DIR})
include_directories(${IMGUIZMO_PARENT_DIR}) # CLion: when I add follow line COMMONS_DIR and VGIZMO_DIR are removed from project and relative contained files not belong to project
include_directories(${IMGUIZMO_DIR})
include_directories(${SRC})

set(SOURCE_FILES
        ${SRC}/oglDebug.cpp
        ${SRC}/oglDebug.h
        ${COMMONS_DIR}/uiMainDlg.cpp
        ${COMMONS_DIR}/uiSettings.cpp
        ${IMGUIZMO_DIR}/vGizmo3D.h
        ${IMGUIZMO_DIR}/vgMath.h
        ${IMGUIZMO_DIR}/imguizmo_quat.h
        ${IMGUIZMO_DIR}/imguizmo_quat.cpp
        ${IMGUI_DIR}/imconfig.h
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui.h
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_impl_opengl3.cpp
        ${IMGUI_DIR}/imgui_impl_opengl3.h
        ${IMGUI_DIR}/imgui_impl_opengl3_loader.h
        ${IMGUI_DIR}/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/imgui_impl_glfw.h
        ${IMGUI_DIR}/imgui_internal.h
        ${IMGUI_DIR}/imstb_rectpack.h
        ${IMGUI_DIR}/imstb_textedit.h
        ${IMGUI_DIR}/imstb_truetype.h
        ${COMMONS_DIR}/shadersAndModel.h)

set(SOURCE_FILES_GLAD
        ${TOOLS_DIR}/glad/glad.cpp
        ${TOOLS_DIR}/glad/glad.h)

if (${BUILD_EMSCRIPTEN})
    project(wglCubeExample)
# -o ${PROJECT_NAME}.html ->  CMAKE_EXECUTABLE_SUFFIX  .html .bc, .js (default), etc
    add_executable(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/wglCube_EMS.cpp ${SOURCE_FILES})

    set(CMAKE_EXECUTABLE_SUFFIX ".html")

# emcc compiler options
    set(M_EMCC_FLAGS "-DGLAPP_NO_OGL_DSA -DGLFW_INCLUDE_ES3")

# BUILD TYPE: [Debug|RelWithDebInfo|Release|MinSizeRel]
    if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
        set(M_EMCC_BINARYEN "-s WASM=0")
        set(M_EMCC_SHELL_FILE "--shell-file \"${COMMONS_DIR}/shell_minimal.html\"")
    else()
        #  -s BINARYEN_METHOD='native-wasm,asmjs' 'interpret-asm2wasm' -s BINARYEN_ASYNC_COMPILATION=0
        if(${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo")
            set(M_EMCC_BINARYEN "-s WASM=1")
            set(M_EMCC_SHELL_FILE "--shell-file \"${COMMONS_DIR}/shell_minimal.html\"")
        else()
            if(${CMAKE_BUILD_TYPE} MATCHES "MinSizeRel")
                # one file no memory option: -s SINGLE_FILE=1 --memory-init-file 0
                set(M_EMCC_BINARYEN "-s WASM=1  -s SINGLE_FILE=1 -s \"BINARYEN_METHOD='native-wasm'\" ")
                set(M_EMCC_SHELL_FILE "--shell-file \"${COMMONS_DIR}/mVeryMinimal.html\"")
            else() # Release
                # -s SINGLE_FILE=1 --memory-init-file 0 -s \"BINARYEN_METHOD='interpret-asm2wasm'\"
                set(M_EMCC_BINARYEN "-s WASM=1 -s \"BINARYEN_METHOD='native-wasm'\" ")
                set(M_EMCC_SHELL_FILE "--shell-file \"${COMMONS_DIR}/mVeryMinimal.html\"")
            endif()
        endif()
    endif()

    set(M_EMCC_OGL_OPTIONS "-s USE_WEBGL2=1 -s USE_GLFW=3 -s LEGACY_GL_EMULATION=0 ")
    set(M_EMCC_OPTIONS " -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=1 -s ALLOW_MEMORY_GROWTH=1 -s PRECISE_F32=1 ${M_EMCC_SHELL_FILE} ${M_EMCC_OGL_OPTIONS} ${M_EMCC_BINARYEN}")

#    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${EMSOUT}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${M_GLOBAL_FLAGS} ${M_EMCC_FLAGS} ${M_EMCC_OPTIONS}")
else()

find_package(glm CONFIG)

option(IMGUIZMO_USES_GLM "Use GLM instead of internal vgMath" OFF)

if(glm_FOUND OR IMGUIZMO_USES_GLM)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVGIZMO_USES_GLM")
endif()

find_package(OpenGL)

if(OPENGL_FOUND)
    message(STATUS "OPENGL_INCLUDE_DIRS: ${OPENGL_INCLUDE_DIRS}")
    message(STATUS "OPENGL_LIBRARY: ${OPENGL_LIBRARY}")

    include_directories(${OPENGL_INCLUDE_DIRS})
else ()
    message (FATAL_ERROR "OPENGL not found... REQUIRED!!!!")
endif(OPENGL_FOUND)

    find_package(glfw3 CONFIG)

    project(oglCubeExample)

    add_executable( ${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/wglCube_EMS.cpp ${SOURCE_FILES} ${SOURCE_FILES_GLFW} ${SOURCE_FILES_GLAD} ${COMMONS_UI_FILES})
    target_link_libraries(${PROJECT_NAME} glfw)

    target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS} ${OPENGL_LIBRARY})
endif()

