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

# Debug Validation Layer is enabled in ONLY Debug mode
#       cmake -DFORCE_VALIDATION_LAYER:BOOL=TRUE
# enable VL anyway
option(FORCE_VALIDATION_LAYER "Enable Debug Validation Layer also in Release build" OFF )

set(CMAKE_INCLUDE_DIRECTORIES_BEFORE, ON)

set(CMAKE_CXX_STANDARD 17)

option(IMGUIZMO_USES_GLM "Use GLM instead of internal vgMath" ON)
# glm_FOUND OR
if(IMGUIZMO_USES_GLM)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVGIZMO_USES_GLM")
endif(IMGUIZMO_USES_GLM)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DIMGUIZMO_VMOD_AXIS_Y=-")


if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
  message(STATUS "CMAKE_BUILD_TYPE not specified: use Debug by default...")
endif(NOT CMAKE_BUILD_TYPE)


if(${CMAKE_BUILD_TYPE} MATCHES "Debug" OR FORCE_VALIDATION_LAYER)
    set(M_GLOBAL_FLAGS "${M_GLOBAL_FLAGS} -DENABLE_VALIDATION_LAYER")
endif()


find_package(glfw3 CONFIG)

set(SRC          ${CMAKE_SOURCE_DIR})
set(COMMONS_DIR  ${SRC}/../commons)
set(TOOLS_DIR    ${CMAKE_SOURCE_DIR}/../../libs)
set(IMGUI_DIR    ${TOOLS_DIR}/imgui)
set(IMGUIZMO_DIR ${CMAKE_SOURCE_DIR}/../../imguizmo_quat)

include_directories(${SRC})
include_directories(${TOOLS_DIR})
include_directories(${IMGUIZMO_DIR})
include_directories(${SRC}/../commons)


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


    set(SOURCE_FILES
        ${SRC}/utils/math.cpp
        ${SRC}/utils/shaders.cpp
        ${SRC}/utils/utils.cpp
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
        ${IMGUI_DIR}/imgui_impl_vulkan.cpp
        ${IMGUI_DIR}/imgui_impl_vulkan.h
        ${IMGUI_DIR}/imgui_internal.h
        ${IMGUI_DIR}/imstb_rectpack.h
        ${IMGUI_DIR}/imstb_textedit.h
        ${IMGUI_DIR}/imstb_truetype.h)

set(SOURCE_FILES_GLFW
        ${IMGUI_DIR}/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/imgui_impl_glfw.h
)
set(SOURCE_FILES_SDL
        ${IMGUI_DIR}/imgui_impl_sdl2.cpp
        ${IMGUI_DIR}/imgui_impl_sdl2.h
)
set(COMMONS_UI_FILES "")


file( GLOB APP_SOURCES ${SRC}/vkCube*.cpp )
foreach( vkCubeSourceFile ${APP_SOURCES} )
    get_filename_component( vkCubeName ${vkCubeSourceFile} NAME_WE )      # Cut off the file extension and directory path
    project(${vkCubeName})

    if(${vkCubeName} MATCHES "Cube_07")
        set(COMMONS_UI_FILES
                ${COMMONS_DIR}/uiMainDlg.cpp
                ${COMMONS_DIR}/uiSettings.cpp
        )
    endif()

    if(${vkCubeName} MATCHES "SDL")
        find_package(SDL2)
        add_executable( ${PROJECT_NAME} ${vkCubeSourceFile} ${SOURCE_FILES} ${SOURCE_FILES_SDL} ${COMMONS_UI_FILES})
        include_directories(${SDL2_INCLUDE_DIRS})
        set(TARGET_LIBS ${CMAKE_DL_LIBS} ${SDL2_LIBRARIES})
    else()
        add_executable( ${PROJECT_NAME} ${vkCubeSourceFile} ${SOURCE_FILES} ${SOURCE_FILES_GLFW} ${COMMONS_UI_FILES})
        target_link_libraries(${PROJECT_NAME} glfw)
        set(TARGET_LIBS ${CMAKE_DL_LIBS} )
    endif()

    target_compile_options(${PROJECT_NAME} PRIVATE -Wno-deprecated-declarations -fpermissive)

    if(VULKAN_FOUND)
        target_include_directories(${PROJECT_NAME} PUBLIC $ENV{VULKAN_SDK}/include)
        target_link_libraries (${PROJECT_NAME} ${Vulkan_LIBRARIES} -lpthread -lX11 -lrt ${VK_ADD_LIBS})
    endif (VULKAN_FOUND)
    
    target_link_libraries(${vkCubeName} ${OPENGL_LIBRARY} ${TARGET_LIBS})
endforeach( vkCubeSourceFile ${APP_SOURCES} )



# target_include_directories(${PROJECT_NAME} PUBLIC .)

#addVulkanLibs(VkLayer_khronos_validation)
#addVulkanLibs(glslang)
#addVulkanLibs(MachineIndependent)
#addVulkanLibs(SPIRV)
#addVulkanLibs(OSDependent)
#addVulkanLibs(GenericCodeGen)
#addVulkanLibs(OGLCompiler)
# it's necessary only to compile shaders from code




