cmake_minimum_required(VERSION 3.17)
project(oglCube)

set(CMAKE_CXX_STANDARD 14)

include_directories(glad)
include_directories(.)

set(SRC          ${CMAKE_SOURCE_DIR})
set(TOOLS_DIR    ${CMAKE_SOURCE_DIR}/../../libs)
set(IMGUI_DIR    ${TOOLS_DIR}/imgui)
set(IMGUIZMO_DIR ${CMAKE_SOURCE_DIR}/../../imguizmo_quat)

find_package(OpenGL)

if(OPENGL_FOUND)
    message(STATUS "OPENGL_INCLUDE_DIRS: ${OPENGL_INCLUDE_DIRS}")
    message(STATUS "OPENGL_LIBRARY: ${OPENGL_LIBRARY}")

    include_directories(${OPENGL_INCLUDE_DIRS})
else ()
    message (FATAL_ERROR "OPENGL not found... REQUIRED!!!!")
endif(OPENGL_FOUND)

find_package(glfw3 REQUIRED)

include_directories(${TOOLS_DIR})
include_directories(${IMGUIZMO_DIR})

set(SOURCE_FILES
        ${SRC}/oglDebug.cpp
        ${SRC}/oglDebug.h
        ${IMGUIZMO_DIR}/imguizmo_quat.h
        ${IMGUIZMO_DIR}/imguizmo_quat.cpp
        ${IMGUIZMO_DIR}/imGuIZMOquat.h
        ${IMGUI_DIR}/imconfig.h
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui.h
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/imgui_impl_glfw.h
        ${IMGUI_DIR}/imgui_impl_opengl3.cpp
        ${IMGUI_DIR}/imgui_impl_opengl3.h
        ${IMGUI_DIR}/imgui_impl_opengl3_loader.h
        ${IMGUI_DIR}/imgui_internal.h
        ${IMGUI_DIR}/imstb_rectpack.h
        ${IMGUI_DIR}/imstb_textedit.h
        ${IMGUI_DIR}/imstb_truetype.h
        ${TOOLS_DIR}/glad/glad.cpp
        ${TOOLS_DIR}/glad/glad.h
)

file( GLOB APP_SOURCES ${SRC}/oglCube*.cpp )
foreach( oglCubeSourceFile ${APP_SOURCES} )
    get_filename_component( oglCubeName ${oglCubeSourceFile} NAME_WE )      # Cut off the file extension and directory path
    add_executable( ${oglCubeName} ${oglCubeSourceFile} ${SOURCE_FILES})

    set(TARGET_LIBS ${CMAKE_DL_LIBS} "-lglfw -lpthread -lX11")
    target_link_libraries(${oglCubeName} ${OPENGL_LIBRARY} ${TARGET_LIBS})
endforeach( oglCubeSourceFile ${APP_SOURCES} )

