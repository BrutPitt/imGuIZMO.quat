cmake_minimum_required(VERSION 3.10)
project(qjSet)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL REQUIRED)

# relative source directory
set(SRC "${CMAKE_SOURCE_DIR}/src")

set(SOURCE_FILES
    ${SRC}/glApp.h
    ${SRC}/glApp.cpp
    ${SRC}/glWindow.h
    ${SRC}/glWindow.cpp
    ${SRC}/qJulia.h
    ${SRC}/qJulia.cpp
    ${SRC}/ui/uiMainDlg.h
    ${SRC}/ui/uiMainDlg.cpp
    ${SRC}/ui/uiSettings.cpp
    ${SRC}/tools/glslProgramObject.cpp
    ${SRC}/tools/glslShaderObject.cpp
    ${SRC}/tools/glslProgramObject.h
    ${SRC}/tools/glslShaderObject.h    
    ${SRC}/tools/imGuIZMO.cpp
    ${SRC}/tools/imGuIZMO.h
    ${SRC}/tools/virtualGizmo.h
    ${SRC}/libs/ImGui/imconfig.h
    ${SRC}/libs/ImGui/imgui.h
    ${SRC}/libs/ImGui/imgui.cpp
    ${SRC}/libs/ImGui/imgui_widgets.cpp
    ${SRC}/libs/ImGui/imgui_draw.cpp
    ${SRC}/libs/ImGui/imgui_demo.cpp
    ${SRC}/libs/ImGui/imgui_impl_glfw.cpp
    ${SRC}/libs/ImGui/imgui_impl_glfw.h
    ${SRC}/libs/ImGui/imgui_impl_opengl3.cpp
    ${SRC}/libs/ImGui/imgui_impl_opengl3.h
    ${SRC}/libs/ImGui/imgui_internal.h
    ${SRC}/libs/ImGui/imstb_rectpack.h
    ${SRC}/libs/ImGui/imstb_textedit.h
    ${SRC}/libs/ImGui/imstb_truetype.h)

# additional INCLUDE directories
include_directories(${SRC})
include_directories(${SRC}/libs)
include_directories(${SRC}/tools)

# additional LIB directories
# link_directories()


set(M_GLOBAL_FLAGS "-DIMGUI_IMPL_OPENGL_ES3 -D_USE_GLFW_ -DGLFW_INCLUDE_NONE -DGLM_ENABLE_EXPERIMENTAL -DGLAPP_NO_OGL_DSA -DGLFW_INCLUDE_ES3 -fpermissive")


if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
#    set(CMAKE_C_COMPILER "emcc")
#    set(CMAKE_CXX_COMPILER "emcc")

# -o ${PROJECT_NAME}.html ->  CMAKE_EXECUTABLE_SUFFIX  .html .bc, .js (default), etc
    set(CMAKE_EXECUTABLE_SUFFIX ".html")

# relative emscripten output files
    set(EMSOUT "emsOut")
    file(MAKE_DIRECTORY ${EMSOUT})
    file(MAKE_DIRECTORY ${EMSOUT}/${CMAKE_BUILD_TYPE})
    set(EMSOUT ${EMSOUT}/${CMAKE_BUILD_TYPE})

# emcc compiler options
    set(M_EMCC_FLAGS "-DGLAPP_NO_OGL_DSA -DGLFW_INCLUDE_ES3")

# BUILD TYPE: [Debug|RelWithDebInfo|Release|MinSizeRel]    
    if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
        set(M_EMCC_BINARYEN "-s WASM=0 -s DEMANGLE_SUPPORT=1 ")
        set(M_EMCC_SHELL_FILE "--shell-file \"${CMAKE_SOURCE_DIR}/shell_minimal.html\"")
    else()
        #  -s BINARYEN_METHOD='native-wasm,asmjs' 'interpret-asm2wasm' -s BINARYEN_ASYNC_COMPILATION=0
        if(${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo")
            set(M_EMCC_BINARYEN "-s WASM=1")
            set(M_EMCC_SHELL_FILE "--shell-file \"${SRC}/shell_minimal.html\"")
        else()
            if(${CMAKE_BUILD_TYPE} MATCHES "MinSizeRel")
                # one file no memory option: -s SINGLE_FILE=1 --memory-init-file 0 
                set(M_EMCC_BINARYEN "-s WASM=1 -s SINGLE_FILE=1 --memory-init-file 0 -s \"BINARYEN_METHOD='native-wasm'\" ")
                set(M_EMCC_SHELL_FILE "--shell-file \"${SRC}/mVeryMinimal.html\"")
            else() # Release 
                # -s SINGLE_FILE=1 --memory-init-file 0 -s \"BINARYEN_METHOD='interpret-asm2wasm'\"
                set(M_EMCC_BINARYEN "-s WASM=1 -s \"BINARYEN_METHOD='native-wasm'\" ")
                set(M_EMCC_SHELL_FILE "--shell-file \"${SRC}/mVeryMinimal.html\"")
            endif()    
        endif()    
    endif()

# grouped EMCC_OPTIONS
    set(M_EMCC_EMBED_FILES "--embed-file \"${CMAKE_SOURCE_DIR}/Shaders/qjFragES2.glsl@Shaders/\" --embed-file \"${CMAKE_SOURCE_DIR}/Shaders/qjVert.glsl@Shaders/\"")
    # set(M_EMCC_EMBED_FILES "--preload-file \"${CMAKE_SOURCE_DIR}/Shaders@/Shaders\"")
    # -s FULL_ES3=1 -s OFFSCREENCANVAS_SUPPORT=1
    set(M_EMCC_OGL_OPTIONS "-s USE_WEBGL2=1 -s USE_GLFW=3 -s LEGACY_GL_EMULATION=0 -s OFFSCREENCANVAS_SUPPORT=1 ")

# merge all EMCC_OPTIONS
#    set(M_EMCC_OPTIONS "-s \"BINARYEN_TRAP_MODE='clamp'\" ${M_EMCC_SHELL_FILE} ${M_EMCC_OGL_OPTIONS} ${M_EMCC_BINARYEN} ${M_EMCC_EMBED_FILES}")
    set(M_EMCC_OPTIONS "-s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=1 -s ALLOW_MEMORY_GROWTH=1 -s BINARYEN_TRAP_MODE=clamp -s PRECISE_F32=1 ${M_EMCC_SHELL_FILE} ${M_EMCC_OGL_OPTIONS} ${M_EMCC_BINARYEN} ${M_EMCC_EMBED_FILES}")

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/${EMSOUT}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${M_GLOBAL_FLAGS} ${M_EMCC_FLAGS} ${M_EMCC_OPTIONS}")

    add_executable(${PROJECT_NAME} ${SOURCE_FILES})

#    set_target_properties(${PROJECT_NAME}  PROPERTIES LINK_FLAGS "${M_EMCC_OPTIONS}")
else()
if(APPLE)
    include_directories(src/libs/glfw/buildOSX/include)
    link_directories(src/libs/glfw/buildOSX/lib)
else()
    if(WIN32)
        include_directories(src/libs/glfw/buildWin/include)
        link_directories(src/libs/glfw/buildWin/lib)
    else()
        include_directories(src/libs/glfw/buildLinux/include)
        link_directories(src/libs/glfw/buildLinux/lib)
    endif(WIN32)
endif(APPLE)

    set(M_CXX_FLAGS "-DAPP_REQUIRE_OGL45")

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${M_GLOBAL_FLAGS} ${M_CXX_FLAGS}")

#   find_package(glfw3 REQUIRED)

    add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${SRC}/libs/glad/glad.cpp)

    target_link_libraries(${PROJECT_NAME} ${OPENGL_gl_LIBRARY} glfw dl)
endif ()
