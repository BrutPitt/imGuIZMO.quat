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
#include <cstdlib>
#include <iostream>
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>
#include <SDL2/SDL.h>

#include "oglDebug.h"
#include "../commons/shadersAndModel.h"

/////////////////////////////////////////////////////////////////////////////
// imGuIZMO: include imGuIZMOquat.h or imguizmo_quat.h
#include <imguizmo_quat/imguizmo_quat.h> // now also imguizmo_quat.h from v3.1

void renderWidgets(vg::vGizmo3D &track, vec3& vLight);

int width = 1280, height = 800;
SDL_Window *sdlWindow = nullptr;
SDL_GLContext gl_context;

const int nVertex = sizeof(coloredCubeData)/(sizeof(float)*2);
GLuint nElemVtx = 4;

// Shaders & Vertex attributes
GLuint program, vao, vaoBuffer;
enum loc { vtxIdx = 0, colIdx, mvpIdx, lightIdx};     // shader locations

mat4 mvpMatrix, viewMatrix, projMatrix;
mat4 lightObj, lightMatrix, cubeObj;

vec3 lightPos(2, 2.5, 3);        // Light Position

/// imGuIZMO / vGizmo3D : declare global/static/member/..
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
vg::vGizmo3D track;     // using vGizmo3D global/static/member instead of specifics variables...
                        // have rotations & Pan/Dolly position variables inside to use with imGuIZMO.quat
                        // And it's necessary if you want use also direct-screen manipulator

mat4 compensateView; // compensate rotation of viewMatrix lookAt Matrix

void draw()
{
    glUseProgram(program);

    glProgramUniformMatrix4fv(program, loc::mvpIdx,   1, false, value_ptr(mvpMatrix)  );  // vgMath permits cast to mat4*
    glProgramUniformMatrix4fv(program, loc::lightIdx, 1, false, value_ptr(lightMatrix));  // using value_ptr maintains GLM compatibility

    glBindVertexArray(vao);
    //glDrawArrays(GL_TRIANGLES, 0, nVertex);
    glDrawArraysInstanced(GL_TRIANGLES, 0, nVertex, 2);   // now using instanced draw to "simulate" light

    glUseProgram(0);
}

void setPerspective()
{
    float aspectRatio = float(height) / float(width);       // Set "camera" position and perspective
    float fov = radians( 45.0f ) * aspectRatio;
    projMatrix = perspective( fov, 1/aspectRatio, 0.1f, 100.0f );
}

void setScene()
{
    viewMatrix = lookAt( vec3( 10.0f, 10.0f, 10.0f ),   // From / EyePos
                         vec3(  0.0f,  0.0f,  0.0f ),   // To   /
                         vec3(  3.0f,  1.0f,   .0f));   // Up

    // Now scale cube to better view light position
    cubeObj = mat4(1); // nothing to do ... scale( vec3(.5));

/// imGuIZMO / vGizmo3D
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //track.setRotation(quat(1,0,0,0));                     // vGizmo3D with NO initial rotation (default initialization)
    //track.setRotation(eulerAngleXYZ(vec3(radians(45),
    //                                     radians( 0),
    //                                     radians( 0))));  // vGizmo3D with rotation of 45 degrees on X axis
    track.setRotation(viewMatrix);                          // vGizmo3D with ViewMatrix (lookAt) rotation

    // for Pan & Dolly always bounded on screen coords (x = left/right, y = up/douw, z = in/out) we remove viewMatrix rotation
    // otherwise Pan & Dolly have as reference the Cartesian axes
    compensateView = inverse(mat4_cast(quat(viewMatrix)));


    // light model

    // acquiring rotation for the light pos
    const float len = length(lightPos);
     //if(len<1.0 && len>= FLT_EPSILON) { normalize(lightPos); len = 1.0; }  // controls are not necessary: lightPos is known
     //else if(len > FLT_EPSILON)
        quat q = angleAxis(acosf(-lightPos.x/len), normalize(vec3(FLT_EPSILON, lightPos.z, -lightPos.y)));
    track.setSecondRot(q);          // store secondary rotation for the Light

    lightObj = translate(mat4(1), lightPos);
    lightObj = scale(lightObj, vec3(.1));       // using same cube vertex but with 10% size

    lightObj = inverse(static_cast<mat4>(track.getSecondRot())) * lightObj ;

    setPerspective();
}

void windowResize(int w, int h)
{
    width = w; height = h;
    setPerspective();
    glViewport(0, 0, width, height);

    track.viewportSize(w, h);   // call it on resize window to re-adjust mouse sensitivity

    draw();
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
    setScene();
}

void initFramework()
{

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return ;
    }

    // GL 3.0 + GLSL 130
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    ///        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);

    sdlWindow = SDL_CreateWindow("glCube", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE| SDL_WINDOW_ALLOW_HIGHDPI);
    gl_context = SDL_GL_CreateContext(sdlWindow);
    SDL_GL_MakeCurrent(sdlWindow, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    gladLoadGL();

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
    ImGui_ImplSDL2_InitForOpenGL(sdlWindow, &gl_context);
    ImGui_ImplOpenGL3_Init("#version 450");
}

/// vGizmo3D initialize: <br>
/// set/associate mouse BUTTON IDs and KEY modifier IDs to vGizmo3D functionalities <br><br>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void initVGizmo3D()     // Settings to control vGizmo3D
{
    // Initialization are necessary to associate your preferences to vGizmo3D
    // These are also the DEFAULT values, so if you want to maintain these combinations you can omit they
    // and to override only the associations that you want modify
        track.setGizmoRotControl         (vg::evButton1  /* or vg::evLeftButton */, 0 /* vg::evNoModifier */ );
    // Rotations around specific axis: mouse button and key modifier
        track.setGizmoRotXControl        (vg::evButton1  /* or vg::evLeftButton */, vg::evShiftModifier);
        track.setGizmoRotYControl        (vg::evButton1  /* or vg::evLeftButton */, vg::evControlModifier);
        track.setGizmoRotZControl        (vg::evButton1  /* or vg::evLeftButton */, vg::evAltModifier | vg::evSuperModifier);
    // Set vGizmo3D control for secondary rotation
        track.setGizmoSecondaryRotControl(vg::evButton2  /* or vg::evRightButton */, 0 /* vg::evNoModifier */ );
    // Pan and Dolly/Zoom: mouse button and key modifier
        track.setDollyControl            (vg::evButton2 /* or vg::evRightButton */, vg::evControlModifier);
        track.setPanControl              (vg::evButton2 /* or vg::evRightButton */, vg::evShiftModifier);
    // N.B. vg::enums are ONLY mnemonic: select and pass specific vg::enum to framework (that can have also different IDs)

    // passing the screen sizes auto-set the mouse sensitivity
        track.viewportSize(width, height);      // but if you need to more feeling with the mouse use:
    // track.setGizmoFeeling(1.0);              // 1.0 default,  > 1.0 more sensible, < 1.0 less sensible

    // setIdleRotSpeed(1.0)                     // If used Idle() feature (continue rotation on Idle) it set that speed: more speed > 1.0 ,  less < 1.0

    // other settings if you need it
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // track.setDollyScale(1.0f);               // > 1.0 more sensible, < 1.0 less sensible
    // track.setDollyPosition(/* your pos */);  // input: float/double or vec3... in vec3 only Z is acquired
    // track.setPanScale(1.0f);                 // > 1.0 more sensible, < 1.0 less sensible
    // track.setPanyPosition(/* your pos */);   // vec3 ==> only X and Y are acquired
    // track.setPosition(/* your pos */);       // input vec3 is equivalent to call: track.setDollyPosition(/* your pos */); and track.setPanyPosition(/* your pos */);
    // track.setRotationCenter(/* vec3 */);     // new rotation center
    //
    // Watch vGizmo.h for more functionalities
}

/// vGizmo3D: Check key modifier currently pressed (GLFW version)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int getModifier(SDL_Window* window = nullptr) {
    SDL_Keymod keyMod = SDL_GetModState();
    if(keyMod & KMOD_CTRL)          return vg::evControlModifier;
    else if(keyMod & KMOD_SHIFT)    return vg::evShiftModifier;
    else if(keyMod & KMOD_ALT)      return vg::evAltModifier;
    else if(keyMod & KMOD_GUI)      return vg::evSuperModifier;
    else return vg::evNoModifier;
}

int main(int /* argc */, char ** /* argv */)    // necessary for SDLmain in Windows
{
    initFramework();         // initialize GLFW framework
    initGL();           // init OpenGL building vaoBuffer and shader program (compile and link vtx/frag shaders)

    // vGizmo3D: initialize
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    initVGizmo3D();

    // other OpenGL settings... used locally
    vec4 bgColor = vec4(0.0f);
    GLfloat f=1.0f;

    // Setup/Initialize Dear ImGui context
    initImGui();
    ImGuiStyle& style = ImGui::GetStyle();

    // imGuIZMO: set mouse feeling and mods
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    imguiGizmo::setGizmoFeelingRot(2.f);                    // default 1.0, >1 more mouse sensitivity, <1 less mouse sensitivity
    imguiGizmo::setPanScale(3.5f);                          // default 1.0, >1 more, <1 less
    imguiGizmo::setDollyScale(3.5f);                        // default 1.0, >1 more, <1 less
    imguiGizmo::setDollyWheelScale(7.0f);                   // default 2.0, > more, < less ... (from v3.1 separate values)
    imguiGizmo::setPanModifier(vg::evControlModifier);      // change KEY modifier: CTRL (default)
    imguiGizmo::setDollyModifier(vg::evShiftModifier);     // change KEY modifier: SHIFT (default)

    SDL_Event event;
    bool done = false;
    // main render/draw loop
    while(!done) {
        while(SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if(event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(sdlWindow))
                done = true;
            if(event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                int w, h;
                SDL_GetWindowSize(sdlWindow, &w, &h);
                windowResize(w, h);
            }
        }
        if (SDL_GetWindowFlags(sdlWindow) & SDL_WINDOW_MINIMIZED) {
            SDL_Delay(10);
            continue;
        }
        glClearBufferfv(GL_DEPTH, 0, &f);
        glClearBufferfv(GL_COLOR, 0, value_ptr(bgColor));


    // vGizmo3D: is necessary intercept mouse event not destined to ImGui
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        if(!ImGui::GetIO().WantCaptureMouse) {
            static bool leftPress = false, rightPress = false;
            int x, y;
            int mouseState = SDL_GetMouseState(&x, &y);
            bool isLPressed = mouseState == SDL_BUTTON_LMASK;
            bool isRPressed = mouseState == SDL_BUTTON_RMASK;
            if(leftPress != isLPressed) {                                   // check if leftButton state is changed
                leftPress = isLPressed ;                                    // set new (different!) state
                track.mouse(vg::evLeftButton, getModifier(sdlWindow),       // send communication to vGizmo3D...
                                              leftPress,  x, y);            // ... checking if a key modifier currently is pressed
            }
            if(rightPress != isRPressed) {                                  // check if rightButton state is changed
                rightPress = isRPressed;                                    // set new (different!) state
                track.mouse(vg::evRightButton, getModifier(sdlWindow),      // send communication to vGizmo3D...
                                               rightPress, x, y);           // ... checking if a key modifier currently is pressed
            }
            track.motion(x,y);
        }

    // vGizmo3D: call it every rendering loop if you want a continue rotation until you do not click on screen
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        track.idle();   // set continuous rotation on Idle: the slow rotation depends on speed of last mouse movements
                        // It can be adjusted from setIdleRotSpeed(1.0) > more speed, < less
                        // It can be stopped by click on screen (without mouse movement)

    // ImGUI: prepare new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

    // ImGui: Your windows here

    // Render ALL ImGuIZMO_quat widgets
        renderWidgets(track, lightPos);

    // transferring the rotation to cube model matrix...
        mat4 modelMatrix = cubeObj * mat4_cast(track.getRotation());

    // Build a "translation" matrix
        mat4 translationMatrix = translate(mat4(1), track.getPosition());      // add translations (pan/dolly) to an identity matrix

    // build MVPs matrices to pass to shader
        mvpMatrix   = projMatrix * viewMatrix * compensateView * translationMatrix * modelMatrix;
        lightMatrix = projMatrix * viewMatrix * compensateView * translationMatrix * translate(mat4(1), lightPos) * scale(mat4(1), vec3(.1));

    // draw the cube, passing matrices to the vtx shader
        draw();

    // ImGui Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(sdlWindow);
    }

    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    // Cleanup OpenGL
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vaoBuffer);
    glDeleteProgram(program);

    // Cleanup Framework
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();

    return EXIT_SUCCESS;
}