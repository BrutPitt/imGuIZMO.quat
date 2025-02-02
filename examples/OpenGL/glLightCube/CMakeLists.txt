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

set(CMAKE_CXX_STANDARD 17)

set(SRC          ${CMAKE_SOURCE_DIR})
set(COMMONS_DIR  ${SRC}/../commons)
set(GIZMO_PARENT_DIR ${SRC}/../../..)
set(TOOLS_DIR  ${GIZMO_PARENT_DIR}/libs)
set(IMGUI_DIR    ${TOOLS_DIR}/imgui)
set(GIZMO_DIR ${GIZMO_PARENT_DIR}/imguizmo_quat)
set(ASSETS_DIR ${SRC}/../../assets)
set(WIDGETS_DIR ${SRC}/../../widgets)

include_directories(${TOOLS_DIR})
include_directories(${COMMONS_DIR})
include_directories(${GIZMO_DIR})
include_directories(${ASSETS_DIR}) # CLion trik: to "include" (show) shaders (outside project folder) into Project tool window: not necessary otherwise
include_directories(${WIDGETS_DIR}) # CLion trik: to "include" (show) shaders (outside project folder) into Project tool window: not necessary otherwise

set(SOURCE_FILES
        ${SRC}/oglCube.h
        ${COMMONS_DIR}/oglDebug.cpp
        ${COMMONS_DIR}/oglDebug.h
        ${ASSETS_DIR}/shadersAndModel.h
        ${GIZMO_DIR}/imguizmo_quat.h
        ${GIZMO_DIR}/imguizmo_quat.cpp
)

set(SOURCE_FILES ${SOURCE_FILES}
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
)

set(SOURCE_FILES ${SOURCE_FILES}
        ${TOOLS_DIR}/glad/glad.cpp
        ${TOOLS_DIR}/glad/glad.h
)

set(SOURCE_FILES_GLFW
        ${IMGUI_DIR}/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/imgui_impl_glfw.h)
set(SOURCE_FILES_SDL
        ${IMGUI_DIR}/imgui_impl_sdl2.cpp
        ${IMGUI_DIR}/imgui_impl_sdl2.h)

set(vkSHADERS_RELATIVE_DIR ../../Shaders/lightCube)
add_compile_definitions("-DAPP_SHADERS_DIR=${vkSHADERS_RELATIVE_DIR}")  # Shaders RELATIVE_DIR is acquired from CMake and passed to code:
include_directories(${vkSHADERS_RELATIVE_DIR}) # CLion trik: to "include" (show) shaders (outside project folder) into Project tool window: not necessary otherwise

find_package(OpenGL)

if(OPENGL_FOUND)
    message(STATUS "OPENGL_INCLUDE_DIRS: ${OPENGL_INCLUDE_DIRS}")
    message(STATUS "OPENGL_LIBRARY: ${OPENGL_LIBRARY}")

    include_directories(${OPENGL_INCLUDE_DIRS})
else ()
    message (FATAL_ERROR "OPENGL not found... REQUIRED!!!!")
endif(OPENGL_FOUND)

file( GLOB APP_SOURCES ${SRC}/oglCube*.cpp )
foreach( oglCubeSourceFile ${APP_SOURCES} )
    get_filename_component( oglCubeName ${oglCubeSourceFile} NAME_WE )      # Cut off the file extension and directory path
        project(${oglCubeName})

    if(${oglCubeName} MATCHES "Cube_07")
        set(COMMONS_UI_FILES
                ${WIDGETS_DIR}/uiMainDlg.cpp
                ${WIDGETS_DIR}/uiSettings.cpp)
    else()
        set(COMMONS_UI_FILES "")
    endif()

    if(${oglCubeName} MATCHES "SDL")
        find_package(SDL2 CONFIG)
        include_directories(${SDL2_INCLUDE_DIRS})

        add_executable( ${PROJECT_NAME} ${oglCubeSourceFile} ${SOURCE_FILES} ${SOURCE_FILES_SDL} ${COMMONS_UI_FILES})
        target_link_libraries(${oglCubeName} ${SDL2_LIBRARIES})
    else()
        find_package(glfw3 CONFIG)

        add_executable( ${PROJECT_NAME} ${oglCubeSourceFile} ${SOURCE_FILES} ${SOURCE_FILES_GLFW} ${COMMONS_UI_FILES})
        target_link_libraries(${oglCubeName} glfw)
    endif()

    target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS} ${OPENGL_LIBRARY})
endforeach( oglCubeSourceFile ${APP_SOURCES} )

