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
project(oglCubeExamples)

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
set(COMMONS_DIR  ${SRC}/../commons)
set(TOOLS_DIR    ${CMAKE_SOURCE_DIR}/../../libs)
set(IMGUI_DIR    ${TOOLS_DIR}/imgui)
set(IMGUIZMO_PARENT_DIR ${CMAKE_SOURCE_DIR}/../..)
set(IMGUIZMO_DIR ${IMGUIZMO_PARENT_DIR}/imguizmo_quat)
#set(IMGUIZMO_DIR ${IMGUIZMO_PARENT_DIR}/imGuIZMO.quat)

include_directories(${TOOLS_DIR})
include_directories(${IMGUIZMO_DIR})
include_directories(${SRC}/../commons)

set(SOURCE_FILES
        ${SRC}/oglDebug.cpp
        ${SRC}/oglDebug.h
        ${COMMONS_DIR}/shadersAndModel.h
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
        ${IMGUI_DIR}/imgui_internal.h
        ${IMGUI_DIR}/imstb_rectpack.h
        ${IMGUI_DIR}/imstb_textedit.h
        ${IMGUI_DIR}/imstb_truetype.h
        ${TOOLS_DIR}/glad/glad.cpp
        ${TOOLS_DIR}/glad/glad.h)


file( GLOB APP_SOURCES ${SRC}/oglCube*.cpp )
foreach( oglCubeSourceFile ${APP_SOURCES} )
    get_filename_component( oglCubeName ${oglCubeSourceFile} NAME_WE )      # Cut off the file extension and directory path
    # project(${oglCubeName})

    if(${oglCubeName} MATCHES "Cube_07")
        set(COMMONS_UI_FILES
                ${COMMONS_DIR}/uiMainDlg.cpp
                ${COMMONS_DIR}/uiSettings.cpp)
    else()
        set(COMMONS_UI_FILES "")
    endif()

    if(${oglCubeName} MATCHES "SDL")
        add_executable( ${oglCubeName} ${oglCubeSourceFile} ${SOURCE_FILES} ${SOURCE_FILES_SDL} ${COMMONS_UI_FILES})
        target_link_libraries(${oglCubeName} ${SDL2_LIBRARIES})
    else()
        add_executable( ${oglCubeName} ${oglCubeSourceFile} ${SOURCE_FILES} ${SOURCE_FILES_GLFW} ${COMMONS_UI_FILES})
        target_link_libraries(${oglCubeName} glfw)
    endif()
    
    target_link_libraries(${oglCubeName} ${CMAKE_DL_LIBS} ${OPENGL_LIBRARY})
endforeach( oglCubeSourceFile ${APP_SOURCES} )

