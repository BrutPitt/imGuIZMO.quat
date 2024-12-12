cmake_minimum_required(VERSION 3.17)
project(glslMandel)

set(CMAKE_CXX_STANDARD 14)

include_directories(glad)
include_directories(.)

set(SRC "${CMAKE_SOURCE_DIR}")

add_executable(glslMandel
        ${SRC}/glad/glad.cpp
        ${SRC}/glad/glad.h
        ${SRC}/Mandelf.cpp
        ${SRC}/oglDebug.cpp
        ${SRC}/oglDebug.h
        ${SRC}/imGuIZMOquat.h
        ${SRC}/imGuIZMOquat.cpp
        ${SRC}/imgui/imconfig.h
        ${SRC}/imgui/imgui.cpp
        ${SRC}/imgui/imgui_widgets.cpp
        ${SRC}/imgui/imgui_tables.cpp
        ${SRC}/imgui/imgui.h
        ${SRC}/imgui/imgui_draw.cpp
        ${SRC}/imgui/imgui_demo.cpp
        ${SRC}/imgui/imgui_impl_glfw.cpp
        ${SRC}/imgui/imgui_impl_glfw.h
        ${SRC}/imgui/imgui_impl_opengl3.cpp
        ${SRC}/imgui/imgui_impl_opengl3.h
        ${SRC}/imgui/imgui_impl_opengl3_loader.h
        ${SRC}/imgui/imgui_internal.h
        ${SRC}/imgui/imstb_rectpack.h
        ${SRC}/imgui/imstb_textedit.h
        ${SRC}/imgui/imstb_truetype.h
)

find_package(OpenGL)


if(OPENGL_FOUND)
    message(STATUS "OPENGL_INCLUDE_DIRS: ${OPENGL_INCLUDE_DIRS}")
    message(STATUS "OPENGL_LIBRARY: ${OPENGL_LIBRARY}")

    include_directories(${OPENGL_INCLUDE_DIRS})
else ()
    message (FATAL_ERROR "OPENGL not found... REQUIRED!!!!")
endif(OPENGL_FOUND)

set(TARGET_LIBS ${CMAKE_DL_LIBS} "-lglfw -lpthread -lX11")

target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARY} ${TARGET_LIBS})