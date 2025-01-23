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
project(vkCube)

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
set(COMMONS_DIR  ${SRC}/../commons)
set(TOOLS_DIR    ${CMAKE_SOURCE_DIR}/../../libs)
set(IMGUI_DIR    ${TOOLS_DIR}/imgui)
set(IMGUIZMO_DIR ${CMAKE_SOURCE_DIR}/../../imguizmo_quat)

include_directories(${SRC})
include_directories(${TOOLS_DIR})
include_directories(${IMGUIZMO_DIR})
include_directories(${SRC}/../commons)


    set(SOURCE_FILES
        ${SRC}/vkCube.h
        ${SRC}/framework.h
        ${SRC}/framework.cpp
        ${SRC}/dbgValidationLayer.h
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


#    set(M_GLOBAL_FLAGS "${M_GLOBAL_FLAGS} -DIMGUI_IMPL_OPENGL_LOADER_GLAD -DGLFW_INCLUDE_NONE -DGLAPP_NO_OGL_DSA")


file( GLOB APP_SOURCES ${SRC}/vkCube_*.cpp )
foreach( vkCubeSourceFile ${APP_SOURCES} )
    get_filename_component( vkCubeName ${vkCubeSourceFile} NAME_WE )      # Cut off the file extension and directory path
    project(${vkCubeName})

    if(${vkCubeName} MATCHES "Cube_07")
        set(SOURCE_FILES ${SOURCE_FILES}
                ${COMMONS_DIR}/uiMainDlg.cpp
                ${COMMONS_DIR}/uiSettings.cpp
        )
    endif()

    if(USE_SDL2)
        find_package(SDL2 REQUIRED)
        set(M_GLOBAL_FLAGS "${M_GLOBAL_FLAGS} -DAPP_USES_SDL2")

        set(SOURCE_FILES ${SOURCE_FILES}
            ${IMGUI_DIR}/imgui_impl_sdl2.cpp
            ${IMGUI_DIR}/imgui_impl_sdl2.h )

        add_executable(${PROJECT_NAME} ${vkCubeSourceFile} ${SOURCE_FILES})
        include_directories(${SDL2_INCLUDE_DIRS})
        target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
    elseif(USE_SDL3)
            find_package(SDL3 REQUIRED)
            set(M_GLOBAL_FLAGS "${M_GLOBAL_FLAGS} -DAPP_USES_SDL3")

            set(SOURCE_FILES ${SOURCE_FILES}
                ${IMGUI_DIR}/imgui_impl_sdl3.cpp
                ${IMGUI_DIR}/imgui_impl_sdl3.h )

            add_executable(${PROJECT_NAME} ${vkCubeSourceFile} ${SOURCE_FILES})
            include_directories(${SDL3_INCLUDE_DIRS})
            target_link_libraries(${PROJECT_NAME} ${SDL3_LIBRARIES})
        else()
            find_package(glfw3 REQUIRED)

            set(SOURCE_FILES ${SOURCE_FILES}
                    ${IMGUI_DIR}/imgui_impl_glfw.cpp
                    ${IMGUI_DIR}/imgui_impl_glfw.h)

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