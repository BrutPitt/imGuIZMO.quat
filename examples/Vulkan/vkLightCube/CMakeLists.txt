#------------------------------------------------------------------------------
#  Copyright (c) 2025 Michele Morrone
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
cmake_minimum_required(VERSION 3.16)
project(imguizmo_vkLightCube)

# To use SDL2 | SDL3 as backend framework (instead of GLFW) type:
#       cmake -DUSE_SDL2=ON
#   or
#       cmake -DUSE_SDL3=ON
# obviously is necessary to have SDL2/SDL3 (+devel package) installed
option(USE_SDL2 "use SDL2 instead of GLFW (default) framework" OFF)
option(USE_SDL3 "use SDL3 instead of GLFW (default) framework" OFF)

# Debug Validation Layer is enabled in ONLY Debug mode
#       cmake -DFORCE_VALIDATION_LAYER=ON
# force dbgVL anyway also in Release build
option(FORCE_VALIDATION_LAYER "force Debug ValidationLayer also in Release build" OFF)

set(CMAKE_INCLUDE_DIRECTORIES_BEFORE, ON)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
  message(STATUS "CMAKE_BUILD_TYPE not specified: use Release by default...")
endif(NOT CMAKE_BUILD_TYPE)

set(SRC          ${CMAKE_SOURCE_DIR})
set(GIZMO_PARENT_DIR ${SRC}/../../..)
set(COMMONS_DIR  ${GIZMO_PARENT_DIR}/commons)
set(TOOLS_DIR  ${GIZMO_PARENT_DIR}/libs)
set(GIZMO_DIR ${GIZMO_PARENT_DIR}/imguizmo_quat)
set(ASSETS_DIR ${SRC}/../../assets)

set(IMGUI_DIR           ${TOOLS_DIR}/imgui)
set(IMGUI_BACKEND_DIR   ${IMGUI_DIR}/backends)

include_directories(${TOOLS_DIR})
#include_directories(${GIZMO_PARENT_DIR}) # CLion bug: when I add follow line COMMONS_DIR and GIZMO_DIR are removed from project and relative contained files: "not belong to project" message
include_directories(${COMMONS_DIR})
include_directories(${GIZMO_DIR})
include_directories(${ASSETS_DIR}) # CLion trik: to "include" (show) shaders (outside project folder) into Project tool window: not necessary otherwise
include_directories(${IMGUI_DIR})


    set(SOURCE_FILES
        ${SRC}/vkCube.cpp
        ${SRC}/vkCube.h
        ${COMMONS_DIR}/utils/framework.h
        ${COMMONS_DIR}/utils/framework.cpp
        ${COMMONS_DIR}/utils/dbgValidationLayer.h
        ${GIZMO_DIR}/imguizmo_quat.h
        ${GIZMO_DIR}/imguizmo_quat.cpp
        ${COMMONS_DIR}/widgets/uiMainDlg.cpp
        ${COMMONS_DIR}/widgets/uiSettings.cpp
    )

set(SOURCE_FILES ${SOURCE_FILES}
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_BACKEND_DIR}/imgui_impl_vulkan.cpp
)

set(SOURCE_FILES_GLFW ${IMGUI_BACKEND_DIR}/imgui_impl_glfw.cpp)
set(SOURCE_FILES_SDL2 ${IMGUI_BACKEND_DIR}/imgui_impl_sdl2.cpp)
set(SOURCE_FILES_SDL3 ${IMGUI_BACKEND_DIR}/imgui_impl_sdl3.cpp)


# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#   SPIR-V shaders

set(vkSHADERS_DIR ${COMMONS_DIR}/shaders)
add_compile_definitions("-DAPP_SHADERS_DIR=${vkSHADERS_DIR}")  # Shaders RELATIVE_DIR is acquired from CMake and passed to code:
include_directories(${vkSHADERS_DIR}) # CLion trik: to "include" (show) shaders (outside project folder) into Project tool window: not necessary otherwise

set(COMPILED_SHADERS_EXT ".spirv")
set(COMPILED_SHADERS_EXT_DBG ".dbg.spirv")

set(vkSHADERS
    ${vkSHADERS_DIR}/vkLightCube.vert
    ${vkSHADERS_DIR}/vkLightCube.frag )

set(SOURCE_FILES ${SOURCE_FILES} ${vkSHADERS})

# shaders sources GLSL
#   glslc -O ==> compile glsl in optimized spv file
#   glslc -g ==> compile spv file with debug info
# compiled SPV
#   spirv-opt ==> optimize previous spv (not optimized)
#   spirv-remap -s ==> strip (also) debug infos


if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(COMPILER_SHADER_OPTION -g)
else()
    set(COMPILER_SHADER_OPTION -O)
endif()
set(COMPILER_SHADER_OPTION ${COMPILER_SHADER_OPTION} -DVULKAN_BUILD)

foreach(vkSHADER ${vkSHADERS})
    if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
        set(COMPILED_SHADER_NAME ${vkSHADER}${COMPILED_SHADERS_EXT_DBG})
    else()
        set(COMPILED_SHADER_NAME ${vkSHADER}${COMPILED_SHADERS_EXT})
    endif()
    add_custom_command(OUTPUT ${COMPILED_SHADER_NAME}
        COMMAND $ENV{VULKAN_SDK}/bin/glslc ${vkSHADER} ${SHADERS_ADDITIONAL_FLAGS} ${COMPILER_SHADER_OPTION} -o ${COMPILED_SHADER_NAME}
        DEPENDS ${vkSHADER}
        COMMENT "glslc -g: Building SPIR-V object ${COMPILED_SHADER_NAME}")
        message(STATUS "Generating build commands for ${COMPILED_SHADER_NAME}")
    list(APPEND COMPILED_vkSHADERS ${COMPILED_SHADER_NAME})
endforeach()

add_custom_target(build_shaders ALL DEPENDS ${COMPILED_vkSHADERS})

#   end SPIR-V shaders
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


set(M_GLOBAL_FLAGS "${M_GLOBAL_FLAGS} -DIMGUI_IMPL_OPENGL_LOADER_GLAD -DGLFW_INCLUDE_NONE -DGLAPP_NO_OGL_DSA")
if(USE_SDL2)
    find_package(SDL2 REQUIRED)
    set(M_GLOBAL_FLAGS "${M_GLOBAL_FLAGS} -DAPP_USES_SDL2")
    add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${SOURCE_FILES_SDL2})
    include_directories(${SDL2_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
elseif(USE_SDL3)
        find_package(SDL3 REQUIRED)
        set(M_GLOBAL_FLAGS "${M_GLOBAL_FLAGS} -DAPP_USES_SDL3")
        add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${SOURCE_FILES_SDL3})
        include_directories(${SDL3_INCLUDE_DIRS})
        target_link_libraries(${PROJECT_NAME} ${SDL3_LIBRARIES})
    else()
        find_package(glfw3 REQUIRED)
        add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${SOURCE_FILES_GLFW})
        target_link_libraries(${PROJECT_NAME} glfw)
endif(USE_SDL2)

if(${CMAKE_BUILD_TYPE} MATCHES "Debug" OR FORCE_VALIDATION_LAYER)
    set(M_GLOBAL_FLAGS "${M_GLOBAL_FLAGS} -DENABLE_VALIDATION_LAYER")
endif()

add_dependencies(${PROJECT_NAME} build_shaders)

target_include_directories(${PROJECT_NAME} PUBLIC .)
target_link_libraries(${PROJECT_NAME} ${MORE_LIBS} ${TARGET_LIBS})

find_package(Vulkan REQUIRED)
message(STATUS "Vulkan found in: ${SHADERC_LIB}")

# it's necessary only to compile shaders from code: e.g. in easy_examples using compileShaders()... not more here: build external SPIR-V
find_library(SHADERC_LIB shaderc_combined $ENV{VULKAN_SDK}/lib)
message(STATUS "Shaderc found in: ${SHADERC_LIB}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${M_GLOBAL_FLAGS}")

if(VULKAN_FOUND)
    target_include_directories(${PROJECT_NAME} PUBLIC $ENV{VULKAN_SDK}/include)
    target_link_libraries (${PROJECT_NAME} ${Vulkan_LIBRARIES} ${CMAKE_DL_LIBS}  ${SHADERC_LIB})
endif (VULKAN_FOUND)

#file(WRITE ${CMAKE_SOURCE_DIR}/.idea/.name ${PROJECT_NAME}) # used to rename a Project in clion (run once)