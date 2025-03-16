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
project(imguizmo_oglCube)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL)

if(OPENGL_FOUND)
    message(STATUS "OPENGL_INCLUDE_DIRS: ${OPENGL_INCLUDE_DIRS}")
    message(STATUS "OPENGL_LIBRARY: ${OPENGL_LIBRARY}")

    include_directories(${OPENGL_INCLUDE_DIRS})
else ()
    message (FATAL_ERROR "OPENGL not found... REQUIRED!!!!")
endif(OPENGL_FOUND)

find_package(glfw3 CONFIG)
find_package(SDL2 CONFIG)
include_directories(${SDL2_INCLUDE_DIRS})


set(SRC          ${CMAKE_SOURCE_DIR})
set(IMGUIZMO_PARENT_DIR ${SRC}/../..)
set(COMMONS_DIR  ${IMGUIZMO_PARENT_DIR}/commons)
set(TOOLS_DIR    ${IMGUIZMO_PARENT_DIR}//libs)
set(IMGUIZMO_DIR ${IMGUIZMO_PARENT_DIR}/imguizmo_quat)

set(IMGUI_DIR           ${TOOLS_DIR}/imgui)
set(IMGUI_BACKEND_DIR   ${IMGUI_DIR}/backends)

include_directories(${TOOLS_DIR})
include_directories(${COMMONS_DIR})
include_directories(${IMGUIZMO_DIR})
include_directories(${IMGUI_DIR})

set(SOURCE_FILES
        ${COMMONS_DIR}/utils/oglDebug.cpp
        ${COMMONS_DIR}/assets/cubePC.h
        ${IMGUIZMO_DIR}/imguizmo_quat.h
        ${IMGUIZMO_DIR}/imguizmo_quat.cpp
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_BACKEND_DIR}/imgui_impl_opengl3.cpp
        ${TOOLS_DIR}/glad/glad.cpp
        ${TOOLS_DIR}/glad/glad.h)

set(SOURCE_FILES_GLFW ${IMGUI_BACKEND_DIR}/imgui_impl_glfw.cpp)
set(SOURCE_FILES_SDL2 ${IMGUI_BACKEND_DIR}/imgui_impl_sdl2.cpp)


file( GLOB APP_SOURCES ${SRC}/oglCube*.cpp )
foreach( oglCubeSourceFile ${APP_SOURCES} )
    get_filename_component( oglCubeName ${oglCubeSourceFile} NAME_WE )      # Cut off the file extension and directory path
    # project(${oglCubeName})

    if(${oglCubeName} MATCHES "Cube_07")
        set(COMMONS_UI_FILES
                ${COMMONS_DIR}/widgets/uiMainDlg.cpp
                ${COMMONS_DIR}/widgets//uiSettings.cpp)
    else()
        set(COMMONS_UI_FILES "")
    endif()

    if(${oglCubeName} MATCHES "SDL")
        add_executable( ${oglCubeName} ${oglCubeSourceFile} ${SOURCE_FILES} ${SOURCE_FILES_SDL2} ${COMMONS_UI_FILES})
        target_link_libraries(${oglCubeName} ${SDL2_LIBRARIES})
    else()
        add_executable( ${oglCubeName} ${oglCubeSourceFile} ${SOURCE_FILES} ${SOURCE_FILES_GLFW} ${COMMONS_UI_FILES})
        target_link_libraries(${oglCubeName} glfw)
    endif()
    
    target_link_libraries(${oglCubeName} ${CMAKE_DL_LIBS} ${OPENGL_LIBRARY})
endforeach( oglCubeSourceFile ${APP_SOURCES} )

# file(WRITE ${CMAKE_SOURCE_DIR}/.idea/.name ${PROJECT_NAME}) # used to rename a Project in clion (run once)