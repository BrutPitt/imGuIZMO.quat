 //------------------------------------------------------------------------------
//  Copyright (c) 2018-2020 Michele Morrone
//  All rights reserved.
//
//  https://michelemorrone.eu - https://BrutPitt.com
//
//  twitter: https://twitter.com/BrutPitt - github: https://github.com/BrutPitt
//
//  mailto:brutpitt@gmail.com - mailto:me@michelemorrone.eu
//
//  This software is distributed under the terms of the BSD 2-Clause license
//------------------------------------------------------------------------------
#include <stdlib.h>
#include <iostream>
#include <glad/glad.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include "oglDebug.h"

/////////////////////////////////////////////////////////////////////////////
// imGuIZMO: include imGuIZMOquat.h or imguizmo_quat.h
#include "imGuIZMOquat.h" // now also imguizmo_quat.h


#define GLSL_VERSION "#version 450\n"

const char* vertex_code = GLSL_VERSION R"(
layout(std140) uniform;
layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 inColor;

layout (location = 2) uniform mat4 mvp;

layout (location = 0) out vec4 outColor;

out vec3 vsPos;


void main()
{
    outColor = inColor;
    gl_Position = mvp * pos;
}
)";

// fragment shader with (C)olor in and (C)olor out
const char* fragment_code = GLSL_VERSION R"(
layout (location = 0) in vec4 color;

layout (location = 0) out vec4 outColor;

const float near = .001;
const float far = 30.0;


void main()
{
    outColor = color;
}
)";

// Shaders attributes
enum { locWSize, locMScale, locMTransp };
GLuint program, vao, vbo;

int width = 1280, height = 800;
float mScaleX = 1.5, mScaleY = 1.5, mTranspX = -.75, mTranspY = 0.0;
const float zoomFactor = .0025;

GLuint nElemVtx = 4;

GLFWwindow *glfwWindow;

struct VertexPC
{
  float x, y, z, w;   // Position
  float r, g, b, a;   // Color
};

static const VertexPC coloredCubeData[] =
{
  // red face
  { -1.0f, -1.0f,  1.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f },
  { -1.0f,  1.0f,  1.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f },
  {  1.0f, -1.0f,  1.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f },
  {  1.0f, -1.0f,  1.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f },
  { -1.0f,  1.0f,  1.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f },
  {  1.0f,  1.0f,  1.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f },
  // green face
  { -1.0f, -1.0f, -1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f },
  {  1.0f, -1.0f, -1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f },
  { -1.0f,  1.0f, -1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f },
  { -1.0f,  1.0f, -1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f },
  {  1.0f, -1.0f, -1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f },
  {  1.0f,  1.0f, -1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f },
  // blue face
  { -1.0f,  1.0f,  1.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f },
  { -1.0f, -1.0f,  1.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f },
  { -1.0f,  1.0f, -1.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f },
  { -1.0f,  1.0f, -1.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f },
  { -1.0f, -1.0f,  1.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f },
  { -1.0f, -1.0f, -1.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f },
  // yellow face
  {  1.0f,  1.0f,  1.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f },
  {  1.0f,  1.0f, -1.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f },
  {  1.0f, -1.0f,  1.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f },
  {  1.0f, -1.0f,  1.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f },
  {  1.0f,  1.0f, -1.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f },
  {  1.0f, -1.0f, -1.0f, 1.0f,    1.0f, 1.0f, 0.0f, 1.0f },
  // magenta face
  {  1.0f,  1.0f,  1.0f, 1.0f,    1.0f, 0.0f, 1.0f, 1.0f },
  { -1.0f,  1.0f,  1.0f, 1.0f,    1.0f, 0.0f, 1.0f, 1.0f },
  {  1.0f,  1.0f, -1.0f, 1.0f,    1.0f, 0.0f, 1.0f, 1.0f },
  {  1.0f,  1.0f, -1.0f, 1.0f,    1.0f, 0.0f, 1.0f, 1.0f },
  { -1.0f,  1.0f,  1.0f, 1.0f,    1.0f, 0.0f, 1.0f, 1.0f },
  { -1.0f,  1.0f, -1.0f, 1.0f,    1.0f, 0.0f, 1.0f, 1.0f },
  // cyan face
  {  1.0f, -1.0f,  1.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f },
  {  1.0f, -1.0f, -1.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f },
  { -1.0f, -1.0f,  1.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f },
  { -1.0f, -1.0f,  1.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f },
  {  1.0f, -1.0f, -1.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f },
  { -1.0f, -1.0f, -1.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f },
};

const int nVertex = sizeof(coloredCubeData)/(sizeof(float)*2);

GLuint vaoBuffer, uBuffer;

mat4 mvpMatrix {1}, viewMatrix {1}, projMatrix {1}; // identity initialized
enum loc { vtxIdx = 0, colIdx, mvpIdx};

void draw()
{

    glUseProgram(program);

    glProgramUniformMatrix4fv(program, loc::mvpIdx, 1, false, (float *)mvpMatrix /* or value_ptr(mvpMatrix) */);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, nVertex);

    glUseProgram(0);

}

void glfwWindowSizeCallback(GLFWwindow* window, int w, int h)
{
    glViewport(0, 0, width, height);

    draw();
}

void initGL()
{

    enableDebugCallback();

    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_code, NULL);
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

    //glNamedBufferSubData(uBuffer, 0, sizeof(mvpMatrix), value_ptr(mvpMatrix));
    //glBindBufferBase(GL_UNIFORM_BUFFER, bind::bindIdx, uBuffer);

    glViewport(0, 0, width, height);

    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glFrontFace(GL_CW);

    glDepthRange(-1.0, 1.0);
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
    //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
    ImGui_ImplOpenGL3_Init("#version 450");
}


int main()
{
    initGLFW();
    initGL();
    initImGui();

    ImGuiStyle& style = ImGui::GetStyle();

    float aspectRatio = float(height) / float(width);
    float fov = radians( 45.0f ) * aspectRatio;
// to use to apply rotation to "model" matrix before create ModelView and ModelViewProjection Matrices ==> mvpcMatrix
    //glm::mat4x4 model      = glm::mat4x4( 1.0f );
    vec3 upVec(.0f, 1.0f, .0f);
    viewMatrix = lookAt( vec3( 0.0f, 0.0f, -10.0f ),  vec3( 0.0f, 0.0f, 0.0f ),  upVec);
    projMatrix = perspective( fov, 1/aspectRatio, 0.1f, 100.0f );

    vec4 bgColor = vec4(0.0f);
    GLfloat f=1.0f;

    static vg::vGizmo3D trackball;
    trackball.setRotation(normalize(quat(viewMatrix)));
    //trackball.setRotation(quat(1,0,0,0));
    trackball.setRotationCenter(vec3(0));


    while (!glfwWindowShouldClose(glfwWindow)) {
        glfwPollEvents();
        glClearBufferfv(GL_DEPTH, 0, &f);
        glClearBufferfv(GL_COLOR, 0, value_ptr(bgColor));

        if (glfwGetWindowAttrib(glfwWindow, GLFW_ICONIFIED) != 0)   {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Start the Dear ImGui frame
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
                                            //ImGuiWindowFlags_NoInputs|
                                              ImGuiWindowFlags_NoScrollbar);
    // imGuIZMO: declare global/static/member/..
    ///////////////////////////////////
            //static quat rotation(1,0,0,0);                                            // quaternion to store rotation...
            static quat rotation(normalize(quat(viewMatrix)));                      // or initialize it with a previous NORMALIZED!! quaternion
            static vec3 position;                                                   // vec3 for position

            //quat qt = trackball.getRotation();
            //vec3 modelPosVec = trackball.getPosition();
            quat qt = rotation;
            vec3 modelPosVec = position;

            ImVec4 oldTex(style.Colors[ImGuiCol_Text]);

            ImGui::SetCursorPos(ImVec2(0,0));
            ImGui::PushItemWidth(widgetSize*.25-2);
            ImGui::TextColored(ImVec4(1,0,0,1), "x: % 1.2f", qt.x); ImGui::SameLine();
            ImGui::TextColored(ImVec4(0,1,0,1), "y: % 1.2f", qt.y); ImGui::SameLine();
            ImGui::TextColored(ImVec4(0,0,1,1), "z: % 1.2f", qt.z); ImGui::SameLine();
            ImGui::TextColored(ImVec4(1,1,1,1), "w: % 1.2f", qt.w);
            ImGui::PopItemWidth();


    // ImGuIZMO.quat widget @ upper right corner widget
    ///////////////////////////////////
            ImGui::gizmo3D("##aaa", qt, widgetSize);                                // if(ImGui::gizmo3D(...) == true) ---> widget has been updated
                                                                                    // it returns "only" a rotation (net of transformations) in base to mouse(x,y) movement
                                                                                    // and add new rotation, obtained from new "delta.xy" mouse motion, to previous one (saved in your global/static/member var)
    // ImGuIZMO.quat with also position
    ///////////////////////////////////
            if(ImGui::gizmo3D("##a01", modelPosVec, qt, widgetSize));
                                                                                    // now you can use it: model / object / camera / light / ...
                                                                                    // transform it in base your exigence (is your task)
                                                                                    // from v.3.1 you now can change starting axes visualization (vConfig.h ==>

    // End Imgui transparent window (container)
            ImGui::End();
            style.WindowBorderSize = prevWindowBorderSize;                          // restore border size
            ImGui::PopStyleColor();                                                 // frame color (pushed)
            ImGui::PopStyleColor();                                                 // Background (pushed)

    // Get Model Matrix with rotation
            rotation = qt;
            position = modelPosVec;
            mat4 modelMatrix = mat4_cast(rotation);                     // get rotation Matrix

            //trackball.setRotation(qt);
            //trackball.setPosition(modelPosVec);
            //mat4 modelMatrix = mat4(1);
            //trackball.applyTransform(modelMatrix);

            mat4 translationMatrix = translate(mat4(1), position);      // add translations (pan/dolly) to an identity matrix
            mat4 newViewMatrix     = translationMatrix * (viewMatrix*mat4_cast(rotation));    // move the "camera" to simulate pan/dolly effect
                                                                        // (obviously in alternative you can move the "model" (scene) or single "object")
            mvpMatrix = projMatrix * newViewMatrix;      // now build MVP matrix to pass to shader

    // now build ModelViewProjection +
            //mvpMatrix = projection * view * (mat4_cast(modelRotation) * model)  ;

        if(glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_LEFT)  == GLFW_PRESS) {}
        else if(glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_RIGHT)  == GLFW_PRESS) {}

        draw();

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(glfwWindow);

    }

    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Cleanup OpenGL
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vaoBuffer);
    //glDeleteBuffers(1, &uBuffer);
    glDeleteProgram(program);

    // Cleanup GLFW
    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
}