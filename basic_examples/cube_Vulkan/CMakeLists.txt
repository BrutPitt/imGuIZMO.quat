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
cmake_minimum_required(VERSION 3.16)
project(ImGuIZMO_Cube)

# To build using only ImGuIZMO.quat without vVirtualGizmo3D (screen manipulator) type  or add option:
#       cmake -DUSE_VIRTUALGIZMO=OFF
option(USE_VIRTUALGIZMO "Use ImGuIZMO.quat with vVirtualGizmo3D (screen manipulator)" ON )

# To use SDL2 | SDL3 as backend (instead of GLFW) type or add option to cmake commandline:
#       cmake -DUSE_SDL2=ON
#   or
#       cmake -DUSE_SDL3=ON
option(USE_SDL2 "Use SDL2 instead of GLFW" OFF )
option(USE_SDL3 "Use SDL3 instead of GLFW (if also SDL2 is ON, SDL2 is selected)" OFF )

# Debug Validation Layer is enabled in ONLY Debug mode
#       cmake -DFORCE_VALIDATION_LAYER:BOOL=TRUE
# enable VL anyway
set(FORCE_VALIDATION_LAYER FALSE CACHE BOOL FALSE)

set(CMAKE_INCLUDE_DIRECTORIES_BEFORE, ON)

set(CMAKE_CXX_STANDARD 17)


if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
  message(STATUS "CMAKE_BUILD_TYPE not specified: use Release by default...")
endif(NOT CMAKE_BUILD_TYPE)

set(IMGUI_DIR libs/imgui)

set(SRC "${CMAKE_SOURCE_DIR}")

    set(SOURCE_FILES
        ${SRC}/DrawCube_02.cpp
        ${SRC}/utils/math.cpp
        ${SRC}/utils/shaders.cpp
        ${SRC}/utils/utils.cpp
        ${SRC}/tools/imGuIZMOquat.cpp
        ${SRC}/tools/imGuIZMOquat.h
        ${SRC}/tools/vGizmo.h
        ${SRC}/tools/vgMath.h
        ${SRC}/tools/vgConfig.h
        ${SRC}/${IMGUI_DIR}/imconfig.h
        ${SRC}/${IMGUI_DIR}/imgui.cpp
        ${SRC}/${IMGUI_DIR}/imgui_widgets.cpp
        ${SRC}/${IMGUI_DIR}/imgui_tables.cpp
        ${SRC}/${IMGUI_DIR}/imgui.h
        ${SRC}/${IMGUI_DIR}/imgui_draw.cpp
        ${SRC}/${IMGUI_DIR}/imgui_demo.cpp
        ${SRC}/${IMGUI_DIR}/imgui_impl_vulkan.cpp
        ${SRC}/${IMGUI_DIR}/imgui_impl_vulkan.h
        ${SRC}/${IMGUI_DIR}/imgui_internal.h
        ${SRC}/${IMGUI_DIR}/imstb_rectpack.h
        ${SRC}/${IMGUI_DIR}/imstb_textedit.h
        ${SRC}/${IMGUI_DIR}/imstb_truetype.h)

include_directories(${SRC})
include_directories(${SRC}/libs)
include_directories(${SRC}/tools)
include_directories(${IMGUI_DIR})

#
#   Compile Shaders
#
set(vkSHADERS_DIR ${CMAKE_SOURCE_DIR}/Shaders)

set(vkSHADERS
    ${vkSHADERS_DIR}/qj.vert
    ${vkSHADERS_DIR}/qj.frag )
#set(COMPILED_vkSHADERS
#    ${vkSHADERS_DIR}/${vkSHADER}.spv
#    ${vkSHADERS_DIR}/${vkSHADER}.spv )

set(SHADERS_ADDITIONAL_FLAGS "")
if(USE_VIRTUALGIZMO)
    set(M_GLOBAL_FLAGS "${M_GLOBAL_FLAGS} -DAPP_USES_VIRTUALGIZMO")
endif()

foreach(vkSHADER ${vkSHADERS})
    # if not debug, optimize!
    if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        list(APPEND OPT_vkSHADERS ${vkSHADER}.spv)
    else()
        list(APPEND DBG_vkSHADERS ${vkSHADER}.dbg.spv)
    endif()
endforeach()


# shaders sources GLSL
#   glslc -O ==> compile glsl in optimized spv file
#   glslc -g ==> compile spv file with debug info
# compiled SPV
#   spirv-opt ==> optimize previous spv (not optimized)
#   spirv-remap -s ==> strip (also) debug infos
foreach(vkSHADER ${vkSHADERS})
    # if not debug, optimize!
    if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        add_custom_command(OUTPUT ${vkSHADER}.spv
            COMMAND $ENV{VULKAN_SDK}/bin/glslc ${vkSHADER} ${SHADERS_ADDITIONAL_FLAGS} -O -o ${vkSHADER}.spv
            DEPENDS ${vkSHADER}
            COMMENT "glslc -O: Building SPIR-V object ${vkSHADER}.spv")
    else()
        add_custom_command(OUTPUT ${vkSHADER}.dbg.spv
            COMMAND $ENV{VULKAN_SDK}/bin/glslc ${vkSHADER} ${SHADERS_ADDITIONAL_FLAGS} -g -o ${vkSHADER}.dbg.spv
            DEPENDS ${vkSHADER}
            COMMENT "glslc -g: Building SPIR-V object ${vkSHADER}.dbg.spv")
    endif()
    message(STATUS "Generating build commands for ${vkSHADER}.spv")
endforeach()

    set(SOURCE_FILES
        ${SOURCE_FILES}
        ${vkSHADERS})

    set(M_GLOBAL_FLAGS "${M_GLOBAL_FLAGS} -DGLFW_INCLUDE_NONE")
    if(USE_SDL2)
        find_package(SDL2 REQUIRED)
        set(SOURCE_FILES
            ${SOURCE_FILES}
            ${SRC}/${IMGUI_DIR}/imgui_impl_sdl2.cpp
            ${SRC}/${IMGUI_DIR}/imgui_impl_sdl2.h)
        set(M_GLOBAL_FLAGS "${M_GLOBAL_FLAGS} -DAPP_USES_SDL2")
        include_directories(${SDL2_INCLUDE_DIRS})
        set(MORE_LIBS ${SDL2_LIBRARIES})
    elseif(USE_SDL3)
            find_package(SDL3 REQUIRED)
            set(SOURCE_FILES
                ${SOURCE_FILES}
                ${SRC}/${IMGUI_DIR}/imgui_impl_sdl3.cpp
                ${SRC}/${IMGUI_DIR}/imgui_impl_sdl3.h)
            set(M_GLOBAL_FLAGS "${M_GLOBAL_FLAGS} -DAPP_USES_SDL3")
            include_directories(${SDL3_INCLUDE_DIRS})
            set(MORE_LIBS ${SDL3_LIBRARIES})
        else()
            find_package(glfw3 REQUIRED)
            set(SOURCE_FILES
                ${SOURCE_FILES}
                ${SRC}/${IMGUI_DIR}/imgui_impl_glfw.cpp
                ${SRC}/${IMGUI_DIR}/imgui_impl_glfw.h)
                set(MORE_LIBS -lglfw)
#      endif(USE_SDL3)
    endif(USE_SDL2)

    if(${CMAKE_BUILD_TYPE} MATCHES "Debug" OR FORCE_VALIDATION_LAYER)
        set(M_GLOBAL_FLAGS "${M_GLOBAL_FLAGS} -DENABLE_VALIDATION_LAYER")
    endif()

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${vkSHADERS} ${DBG_vkSHADERS} ${OPT_vkSHADERS})


target_include_directories(${PROJECT_NAME} PUBLIC .)
target_link_libraries(${PROJECT_NAME} ${MORE_LIBS} ${TARGET_LIBS})

find_package(Vulkan REQUIRED)

set(VK_ADD_LIBS "")
macro(addVulkanLibs ARG)
    message("ARG ${ARG}")
    find_library(${ARG}_LIB NAMES ${ARG} PATHS $ENV{VULKAN_SDK}/lib)
    message(STATUS "${ARG} found in: ${${ARG}_LIB}\n")
    set(VK_ADD_LIBS ${VK_ADD_LIBS} ${${ARG}_LIB})
endmacro()

addVulkanLibs(shaderc_combined)
addVulkanLibs(glslang-default-resource-limits)
#addVulkanLibs(VkLayer_khronos_validation)
#addVulkanLibs(glslang)
#addVulkanLibs(MachineIndependent)
#addVulkanLibs(SPIRV)
#addVulkanLibs(OSDependent)
#addVulkanLibs(GenericCodeGen)
#addVulkanLibs(OGLCompiler)
# it's necessary only to compile shaders from code


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${M_GLOBAL_FLAGS}")
target_compile_options(${PROJECT_NAME} PRIVATE -Wno-deprecated-declarations -fpermissive)

if( WIN32 )
    target_compile_definitions( ${PROJECT_NAME} PUBLIC VK_USE_PLATFORM_WIN32_KHR )
elseif( APPLE )
    target_compile_definitions( ${PROJECT_NAME} PUBLIC VK_USE_PLATFORM_METAL_EXT )
elseif( UNIX )
    target_compile_definitions( ${PROJECT_NAME} PUBLIC VK_USE_PLATFORM_XCB_KHR )
else()
    message( FATAL_ERROR, "Vulkan-Hpp: unhandled platform!" )
endif()

if(EXISTS $ENV{RAMDISK}) #my RAMDISK env
    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY $ENV{RAMDISK}/${PROJECT_NAME})
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

if(VULKAN_FOUND)
    target_include_directories(${PROJECT_NAME} PUBLIC $ENV{VULKAN_SDK}/include)
    target_link_libraries (${PROJECT_NAME} ${Vulkan_LIBRARIES} -lpthread -lX11 -lrt ${VK_ADD_LIBS})
endif (VULKAN_FOUND)
