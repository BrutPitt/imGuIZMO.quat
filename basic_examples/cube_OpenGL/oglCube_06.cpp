//------------------------------------------------------------------------------
//  Copyright (c) 2018-2025 Michele Morrone
//  All rights reserved.
//
//  https://michelemorrone.eu - https://brutpitt.com
//
//  X: https://x.com/BrutPitt - GitHub: https://github.com/BrutPitt
//
//  direct mail: brutpitt(at)gmail.com - me(at)michelemorrone.eu
//
//  This software is distributed under the terms of the BSD 2-Clause license
//------------------------------------------------------------------------------
#include <stdlib.h>
#include <iostream>
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

#include "oglDebug.h"
#include "shadersAndModel.h"

/////////////////////////////////////////////////////////////////////////////
// imGuIZMO: include imGuIZMOquat.h or imguizmo_quat.h
#include <imguizmo_quat.h> // now also imguizmo_quat.h from v3.1

int width = 1280, height = 800;
GLFWwindow *glfwWindow;

const int nVertex = sizeof(coloredCubeData)/(sizeof(float)*2);
GLuint nElemVtx = 4;

// Shaders & Vertex attributes
GLuint program, vao, vaoBuffer;

mat4 mvpMatrix, viewMatrix, projMatrix, lightMatrix;
vec3 light(2.5, 2.5, 2.5);
enum loc { vtxIdx = 0, colIdx, mvpIdx, lightIdx};     // shader locations

// vGizmo3D: global/static/member/..
///////////////////////////////////
static vg::vGizmo3D track;

void draw()
{
    glUseProgram(program);

    glProgramUniformMatrix4fv(program, loc::mvpIdx,   1, false, value_ptr(mvpMatrix)  );  // using vgMath instead of GLM you can cast to mat4*
    glProgramUniformMatrix4fv(program, loc::lightIdx, 1, false, value_ptr(lightMatrix));  // vgMath also cast: (mat4*) or static_cast<mat4*>()

    glBindVertexArray(vao);
    //glDrawArrays(GL_TRIANGLES, 0, nVertex);
    glDrawArraysInstanced(GL_TRIANGLES, 0, nVertex, 2);   // now using instanced draw to "simulate" light

    glUseProgram(0);
}

void setCamera()
{
    float aspectRatio = float(height) / float(width);       // Set "camera" position and perspective
    float fov = radians( 75.0f ) * aspectRatio;
    vec3 upVec(0.0f, 1.0f, .0f);
    viewMatrix = lookAt( vec3( 0.0f, 0.0f, 10.0f ),  vec3( 0.0f, 0.0f, 0.0f ),  upVec);
    projMatrix = perspective( fov, 1/aspectRatio, 0.1f, 100.0f );
}

void glfwWindowSizeCallback(GLFWwindow* window, int w, int h)
{
    width = w; height = h;
    setCamera();
    glViewport(0, 0, width, height);
    // call it on resize window to re-align mouse sensitivity
    track.viewportSize(w, h);

    draw();
}

void glfwScrollCallback(GLFWwindow* window, double x, double y)
{
    ImGui_ImplGlfw_ScrollCallback(window, x, y);
    if(ImGui::GetIO().WantCaptureMouse) return;
    track.wheel(x, y);
}


void initGL()
{
    enableDebugCallback();

    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_instanced, NULL);
    glCompileShader(vertex);
    glCompileShader(vertex);

    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_code, NULL);
    glCompileShader(fragment);
    checkShader(fragment);

    program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    checkProgram(program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    glCreateVertexArrays(1, &vao);
    glCreateBuffers(1, &vaoBuffer);
    glNamedBufferStorage(vaoBuffer, sizeof(coloredCubeData), coloredCubeData, 0);

    glVertexArrayAttribBinding(vao,loc::vtxIdx, 0);
    glVertexArrayAttribFormat(vao, loc::vtxIdx, nElemVtx, GL_FLOAT, GL_FALSE, 0);
    glEnableVertexArrayAttrib(vao, loc::vtxIdx);

    glVertexArrayAttribBinding(vao,loc::colIdx, 0);
    glVertexArrayAttribFormat(vao, loc::colIdx, nElemVtx, GL_FLOAT, GL_FALSE, nElemVtx*sizeof(float));
    glEnableVertexArrayAttrib(vao, loc::colIdx);

    glVertexArrayVertexBuffer(vao, 0, vaoBuffer, 0, 2*nElemVtx*sizeof(float));

    glViewport(0, 0, width, height);

    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glFrontFace(GL_CW);

    glDepthRange(-1.0, 1.0);
    setCamera();
}

void initGLFW()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);

    glfwWindow = glfwCreateWindow(width, height, "glCube", NULL, NULL);
    if (!glfwWindow)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(glfwWindow);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);  //get OpenGL extensions

    glfwSetWindowSizeCallback(glfwWindow, glfwWindowSizeCallback);
    glfwSetScrollCallback(glfwWindow, glfwScrollCallback);

    glfwSwapInterval(1); // 0 vSync off - 1 vSync on
}

void initImGui()
{
        // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
    ImGui_ImplOpenGL3_Init("#version 450");
}

// vGizmo3D: initialize
///////////////////////////////////
void initVGizmo3D()     // Settings to control vGizmo3D
{
    // Initialization are necessary to associate specific (GLFW/SDL/etc) frameworks DEFINES/values to control
        track.setGizmoRotControl( (vgButtons) GLFW_MOUSE_BUTTON_LEFT,  (vgModifiers) 0 /* evNoModifier */ );
    // Rotations around specific axis: mouse button and key modifier
        track.setGizmoRotXControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT,  (vgModifiers) GLFW_MOD_SHIFT);
        track.setGizmoRotYControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT,  (vgModifiers) GLFW_MOD_CONTROL);
        track.setGizmoRotZControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT,  (vgModifiers) GLFW_MOD_ALT     | GLFW_MOD_SUPER);
    // Pan and Dolly/Zoom: mouse button and key modifier
        track.setDollyControl(    (vgButtons) GLFW_MOUSE_BUTTON_RIGHT, (vgModifiers) 0 /* evNoModifier */);
        track.setPanControl(      (vgButtons) GLFW_MOUSE_BUTTON_RIGHT, (vgModifiers) GLFW_MOD_CONTROL | GLFW_MOD_SHIFT);
    // passing the screen sizes auto-set the mouse sensitivity
        track.viewportSize(width, height);      // but if you need to more feeling with the mouse use:
    // track.setGizmoFeeling(1.0);              // 1.0 default,  > 1.0 more sensible, < 1.0 less sensible

    // setIdleRotSpeed(1.0)                     // If used Idle() feature (continue rotation on Idle) it set that speed: more speed > 1.0 ,  less < 1.0

    // other settings if you need it
    /////////////////////////////////////////////
    // track.setDollyScale(1.0f);               // > 1.0 more sensible, < 1.0 less sensible
    // track.setDollyPosition(/* your pos */);  // input: float/double or vec3... in vec3 only Z is acquired
    // track.setPanScale(1.0f);                 // > 1.0 more sensible, < 1.0 less sensible
    // track.setPanyPosition(/* your pos */);   // vec3 ==> only X and Y are acquired
    // track.setPosition(/* your pos */);       // input vec3 is equivalent to call: track.setDollyPosition(/* your pos */); and track.setPanyPosition(/* your pos */);
    // track.setRotationCenter(/* vec3 */);     // new rotation center
    //
    // Watch vGizmo.h for more functionalities
}

// vGizmo3D: Useful function to check modifier key currently pressed (GLFW version)
///////////////////////////////////
int getModifier(GLFWwindow* window) {
    if((glfwGetKey(window,GLFW_KEY_LEFT_CONTROL)    == GLFW_PRESS) || (glfwGetKey(window,GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS))
            return GLFW_MOD_CONTROL;
    else if((glfwGetKey(window,GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) || (glfwGetKey(window,GLFW_KEY_RIGHT_SHIFT)   == GLFW_PRESS))
            return GLFW_MOD_SHIFT;
    else if((glfwGetKey(window,GLFW_KEY_LEFT_ALT)   == GLFW_PRESS) || (glfwGetKey(window,GLFW_KEY_RIGHT_ALT)     == GLFW_PRESS))
            return GLFW_MOD_ALT;
    else return 0;
}

int main()
{
    initGLFW();         // initialize GLFW framework
    initGL();           // init OpenGL building vaoBuffer and shader program (compile and link vtx/frag shaders)

    // vGizmo3D: initialize
    ///////////////////////////////////
    initVGizmo3D();

    // other OpenGL settings... used locally
    vec4 bgColor = vec4(0.0f);
    GLfloat f=1.0f;

    // Setup/Initialize Dear ImGui context
    initImGui();
    ImGuiStyle& style = ImGui::GetStyle();

    // imGuIZMO: set mouse feeling and mods
    ///////////////////////////////////
    imguiGizmo::setGizmoFeelingRot(2.f);                    // default 1.0, >1 more mouse sensitivity, <1 less mouse sensitivity
    imguiGizmo::setPanScale(3.5f);                          // default 1.0, >1 more, <1 less
    imguiGizmo::setDollyScale(3.5f);                        // default 1.0, >1 more, <1 less
    imguiGizmo::setDollyWheelScale(7.0f);                   // default 2.0, > more, < less ... (from v3.1 separate values)
    imguiGizmo::setPanModifier(vg::evControlModifier);      // change KEY modifier: CTRL (default)
    imguiGizmo::setDollyModifier(vg::evShiftModifier);     // change KEY modifier: SHIFT (default)

    // main render/draw loop
    while (!glfwWindowShouldClose(glfwWindow)) {
        glfwPollEvents();
        glClearBufferfv(GL_DEPTH, 0, &f);
        glClearBufferfv(GL_COLOR, 0, value_ptr(bgColor));

    // vGizmo3D: is necessary intercept mouse event not destined to ImGui
    ///////////////////////////////////
        if(!ImGui::GetIO().WantCaptureMouse) {
            static int leftPress = 0, rightPress = 0;
            double x, y;
            glfwGetCursorPos(glfwWindow, &x, &y);
            if(glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_LEFT) != leftPress) {           // check if leftButton state is changed
                leftPress = leftPress == GLFW_PRESS ? GLFW_RELEASE : GLFW_PRESS;                // set new (different!) state
                track.mouse((vgButtons)GLFW_MOUSE_BUTTON_LEFT,                                  // send communication to vGizmo3D...
                            (vgModifiers) getModifier(glfwWindow), leftPress, (int)x, (int)y);  // ... checking if a key modifier currently is pressed
            }
            if(glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_RIGHT) != rightPress) {         // same thing for rightButton
                rightPress = rightPress == GLFW_PRESS ? GLFW_RELEASE : GLFW_PRESS;
                track.mouse((vgButtons)GLFW_MOUSE_BUTTON_RIGHT,
                            (vgModifiers) getModifier(glfwWindow), rightPress, (int)x, (int)y);
            }
            if(leftPress == GLFW_PRESS || rightPress == GLFW_PRESS)
                track.motion((float)x,(float)y);                                                // if one button is pressed vGizmo3D catch the motion
        }
    // vGizmo3D: call it every rendering loop if you want a continue rotation until you do not click on screen
    ///////////////////////////////////
        track.idle();   // set continuous rotation on Idle: the smooth rotation depends on speed of last mouse movements
                        // It can be adjusted from setIdleRotSpeed(1.0) > more speed, < less
                        // It can be stopped by click on screen (without mouse movement)


        if (glfwGetWindowAttrib(glfwWindow, GLFW_ICONIFIED) != 0)   {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

    // ImGUI: prepare new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

    // ImGui: Your windows here

    // Imgui window: build a transparent window (container) to insert widget
        float widgetSize=240;
        ImGui::SetNextWindowSize(ImVec2(widgetSize, height), ImGuiCond_Always); // top ...
        ImGui::SetNextWindowPos(ImVec2(width-widgetSize, 0), ImGuiCond_Always); // ... right aligned

        ImGui::PushStyleColor(ImGuiCol_WindowBg,ImVec4(0.f,0.f,0.f,0.f));       // transparent Wnd Background
        ImGui::PushStyleColor(ImGuiCol_FrameBg,ImVec4(0.f,0.f,0.f,0.f));        // transparent frame Background
        const float prevWindowBorderSize = style.WindowBorderSize;              // Save current border size...
        style.WindowBorderSize = .0f;                                           // ... to draw the window with ZERO broder

        bool isVisible = true;                                                  // visibility flag: always on
        ImGui::Begin("##giz", &isVisible, ImGuiWindowFlags_NoTitleBar|          // noTitle / noResize / Noscrollbar
                                          ImGuiWindowFlags_NoResize|
                                          ImGuiWindowFlags_NoScrollbar);

    // imGuIZMO / vGizmo3D: declare global/static/member/..
    ///////////////////////////////////
        //static vg::vGizmo3D track;        // using vGizmo3D global/static/member instead of single variables...
        //                                  // no real advantage for imGuIZMO.quat, but necessary if you want use also direct-screen manipulator
        //  declaration moved UP            // you can initialize it with: setRotation(quat) and setPosition(vec3)

        quat rotation = track.getRotation(); // no more global/static/member but get actual rotation before widget
        vec3 position = track.getPosition(); // no more global/static/member but get actual position before widget
        vec3 tmpLight = -light;              // Light Vector have inverse direction (toward origin) so using a tmp vect

    // colored text for display quat(w,x,y,z) components
        ImGui::SetCursorPos(ImVec2(0,0));
        ImGui::PushItemWidth(widgetSize*.25-2);
        ImGui::TextColored(ImVec4(1,1,1,1), "w: % 1.2f", rotation.w); ImGui::SameLine();
        ImGui::TextColored(ImVec4(1,0,0,1), "x: % 1.2f", rotation.x); ImGui::SameLine();
        ImGui::TextColored(ImVec4(0,1,0,1), "y: % 1.2f", rotation.y); ImGui::SameLine();
        ImGui::TextColored(ImVec4(0,0,1,1), "z: % 1.2f", rotation.z);
        ImGui::PopItemWidth();

    // ImGuIZMO.quat widget
    ///////////////////////////////////
        if(ImGui::gizmo3D("##aaa", rotation, tmpLight, widgetSize)) // if(ImGui::gizmo3D(...) == true) ---> widget has been updated
            light = -tmpLight;                                      // restore sign from acquired rotation

    // ImGuIZMO.quat with also pan and Dolly/zoom
    ///////////////////////////////////
        ImGui::gizmo3D("##a01", position, rotation, widgetSize);    // Ctrl+LButton = Pan ... Shift+LButton = Dolly/Zoom

        track.setRotation(rotation); // and restore rotation after widget
        track.setPosition(position); // and restore position after widget


    // End Imgui window (container) block
        ImGui::End();
        style.WindowBorderSize = prevWindowBorderSize;              // restore border size
        ImGui::PopStyleColor();                                     // frame color (pushed)
        ImGui::PopStyleColor();                                     // Background (pushed)


    // Build a "translation" matrix
        mat4 translationMatrix = translate(mat4(1), track.getPosition());      // add translations (pan/dolly) to an identity matrix
    // build MVP matrix to pass to shader
        mvpMatrix   = projMatrix * translationMatrix * viewMatrix * static_cast<mat4>(track.getRotation());
        lightMatrix = projMatrix * translationMatrix * translate(mat4(1), light) * viewMatrix;

    // draw the cube, passing MVP matrix to the vtx shader
        draw();

    // ImGui Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(glfwWindow);
    }

    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Cleanup OpenGL
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vaoBuffer);
    glDeleteProgram(program);

    // Cleanup GLFW
    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
}