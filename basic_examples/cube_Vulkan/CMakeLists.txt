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
project(imguizmo_vkCube)

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
        ${COMMONS_DIR}/utils/framework.h
        ${COMMONS_DIR}/utils/framework.cpp
        ${COMMONS_DIR}/utils/dbgValidationLayer.h
        ${COMMONS_DIR}/assets/cubePC.h
        ${IMGUIZMO_DIR}/imguizmo_quat.h
        ${IMGUIZMO_DIR}/imguizmo_quat.cpp
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_BACKEND_DIR}/imgui_impl_vulkan.cpp
        ${TOOLS_DIR}/glad/glad.cpp
        ${TOOLS_DIR}/glad/glad.h)

set(SOURCE_FILES_GLFW ${IMGUI_BACKEND_DIR}/imgui_impl_glfw.cpp)
set(SOURCE_FILES_SDL2 ${IMGUI_BACKEND_DIR}/imgui_impl_sdl2.cpp)
set(SOURCE_FILES_SDL3 ${IMGUI_BACKEND_DIR}/imgui_impl_sdl3.cpp)

file( GLOB APP_SOURCES ${SRC}/vkCube_*.cpp )
foreach( vkCubeSourceFile ${APP_SOURCES} )
    get_filename_component( vkCubeName ${vkCubeSourceFile} NAME_WE )      # Cut off the file extension and directory path
    project(${vkCubeName})

    if(${vkCubeName} MATCHES "Cube_07")
        set(SOURCE_FILES ${SOURCE_FILES}
                ${COMMONS_DIR}/widgets/uiMainDlg.cpp
                ${COMMONS_DIR}/widgets/uiSettings.cpp
        )
    endif()

    if(USE_SDL2)
        find_package(SDL2 REQUIRED)
        set(M_GLOBAL_FLAGS "${M_GLOBAL_FLAGS} -DAPP_USES_SDL2")

        set(SOURCE_FILES ${SOURCE_FILES} ${SOURCE_FILES_SDL2})

        add_executable(${PROJECT_NAME} ${vkCubeSourceFile} ${SOURCE_FILES})
        include_directories(${SDL2_INCLUDE_DIRS})
        target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
    elseif(USE_SDL3)
            find_package(SDL3 REQUIRED)
            set(M_GLOBAL_FLAGS "${M_GLOBAL_FLAGS} -DAPP_USES_SDL3")

            set(SOURCE_FILES ${SOURCE_FILES} ${SOURCE_FILES_SDL3})

            add_executable(${PROJECT_NAME} ${vkCubeSourceFile} ${SOURCE_FILES})
            include_directories(${SDL3_INCLUDE_DIRS})
            target_link_libraries(${PROJECT_NAME} ${SDL3_LIBRARIES})
        else()
            find_package(glfw3 REQUIRED)

            set(SOURCE_FILES ${SOURCE_FILES} ${SOURCE_FILES_GLFW})

            add_executable(${PROJECT_NAME} ${vkCubeSourceFile} ${SOURCE_FILES})
            target_link_libraries(${PROJECT_NAME} glfw)
    endif(USE_SDL2)

    if(${CMAKE_BUILD_TYPE} MATCHES "Debug" OR FORCE_VALIDATION_LAYER)
        set(M_GLOBAL_FLAGS "${M_GLOBAL_FLAGS} -DENABLE_VALIDATION_LAYER")
    endif()


    target_include_directories(${PROJECT_NAME} PUBLIC .)
    target_link_libraries(${PROJECT_NAME} ${MORE_LIBS} ${TARGET_LIBS})

    find_package(Vulkan REQUIRED)
    message(STATUS "Vulkan found in: ${SHADERC_LIB}")

    # it's necessary only to compile shaders from code
    find_library(SHADERC_LIB shaderc_combined $ENV{VULKAN_SDK}/lib)
    message(STATUS "Shaderc found in: ${SHADERC_LIB}")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${M_GLOBAL_FLAGS}")
    #target_compile_options(${PROJECT_NAME} PRIVATE -Wno-deprecated-declarations -fpermissive)
    #if(EXISTS $ENV{RAMDISK}) #my RAMDISK env
    #    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY $ENV{RAMDISK}/${PROJECT_NAME})
    #else()
    #    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
    #endif()

    if(VULKAN_FOUND)
        target_include_directories(${PROJECT_NAME} PUBLIC $ENV{VULKAN_SDK}/include)
        target_link_libraries (${PROJECT_NAME} ${Vulkan_LIBRARIES} ${CMAKE_DL_LIBS}  ${SHADERC_LIB})
    endif (VULKAN_FOUND)
endforeach( vkCubeSourceFile ${APP_SOURCES} )

#file(WRITE ${CMAKE_SOURCE_DIR}/.idea/.name ${PROJECT_NAME}) # used to rename a Project in clion (run once)