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

set(APP_NAME imguizmo_wglLightCube)

option(BUILD_EMSCRIPTEN "Build EMSCRIPTEN" ON)

set(CMAKE_CXX_STANDARD 17)

set(SRC          ${CMAKE_SOURCE_DIR})
set(GIZMO_PARENT_DIR ${SRC}/../../../..)
set(COMMONS_DIR  ${GIZMO_PARENT_DIR}/commons)
set(GIZMO_DIR ${GIZMO_PARENT_DIR}/imguizmo_quat)
set(TOOLS_DIR  ${GIZMO_PARENT_DIR}/libs)
set(IMGUI_DIR    ${TOOLS_DIR}/imgui)
set(ASSETS_DIR ${COMMONS_DIR}/assets)
set(WIDGETS_DIR ${COMMONS_DIR}/widgets)

set(IMGUI_DIR           ${TOOLS_DIR}/imgui)
set(IMGUI_BACKEND_DIR   ${IMGUI_DIR}/backends)

include_directories(${TOOLS_DIR})
include_directories(${COMMONS_DIR})
include_directories(${GIZMO_PARENT_DIR}) # CLion: when I add follow line COMMONS_DIR and VGIZMO_DIR are removed from project and relative contained files not belong to project
include_directories(${GIZMO_DIR})
include_directories(${SRC})
include_directories(${ASSETS_DIR}) # CLion trik: to "include" (show) shaders (outside project folder) into Project tool window: not necessary otherwise
include_directories(${IMGUI_DIR})


set(SOURCE_FILES
        ${COMMONS_DIR}/utils/oglDebug.cpp
        ${ASSETS_DIR}/cubePNC.h
        ${GIZMO_DIR}/imguizmo_quat.h
        ${GIZMO_DIR}/imguizmo_quat.cpp
        ${WIDGETS_DIR}/uiMainDlg.cpp
        ${WIDGETS_DIR}/uiSettings.cpp
)

set(SOURCE_FILES ${SOURCE_FILES}
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_BACKEND_DIR}/imgui_impl_opengl3.cpp
        ${IMGUI_BACKEND_DIR}/imgui_impl_glfw.cpp
)

set(SOURCE_FILES_GLAD
        ${TOOLS_DIR}/glad/glad.cpp
        ${TOOLS_DIR}/glad/glad.h)

if (${BUILD_EMSCRIPTEN})
    project(wglLightCube)
# -o ${PROJECT_NAME}.html ->  CMAKE_EXECUTABLE_SUFFIX  .html .bc, .js (default), etc
    add_executable(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/wglCube_EMS.cpp ${SOURCE_FILES})

    set(CMAKE_EXECUTABLE_SUFFIX ".html")

# emcc compiler options
    set(M_EMCC_FLAGS "-DGLAPP_NO_OGL_DSA -DGLFW_INCLUDE_ES3")

# BUILD TYPE: [Debug|RelWithDebInfo|Release|MinSizeRel]
    if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
        set(M_EMCC_BINARYEN "-s WASM=0")
        set(M_EMCC_SHELL_FILE "--shell-file \"${SRC}/shell_minimal.html\"")
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

#file(WRITE ${CMAKE_SOURCE_DIR}/.idea/.name ${PROJECT_NAME}) # used to rename a Project in clion (run once)