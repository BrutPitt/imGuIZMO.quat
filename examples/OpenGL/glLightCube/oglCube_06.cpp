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
#include "oglCube.h"

#include <GLFW/glfw3.h>

#include "oglDebug.h"
#include "../../assets/shadersAndModel.h"

/////////////////////////////////////////////////////////////////////////////
// vGizmo3D:
#include <vGizmo3D.h>

#define FRAG_NAME "vkLightCube.frag"
#define VERT_NAME "vkLightCube.vert"

#define _STRING(x) #x
#define STRING(x) _STRING(x)

int width = 1280, height = 800;
GLFWwindow *glfwWindow;

const int nElemVtx = 4;
const size_t nVertex = sizeof(cubeData)/sizeof(cubeData[0]);

// Shaders & Vertex attributes
GLuint program, vao, vaoBuffer;
enum loc { vtxIdx = 0, nrmIdx, colIdx};     // shader locations
enum bind { matIdx = 0, fgtIdx };

// I maintain this "old" matrices (from easy_examples) to better show the transformations steps and UBO assignments
mat4 mvpMatrix, viewMatrix, projMatrix;
mat4 lightObj, lightMatrix, cubeObj;

struct _uboMat {
    mat4 projMatrix;
    mat4 viewMatrix;
    mat4 modelMatrix;
    mat4 compMatrix;
    mat4 lightMatrix;
    alignas(sizeof(vec4)) vec3 lightPos;
    alignas(sizeof(vec4)) vec3 PoV;
} uboMat;

uniformBlocksClass ubo;

/// imGuIZMO / vGizmo3D : declare global/static/member/..
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
vg::vGizmo3D track;     // using vGizmo3D global/static/member instead of specifics variables...
                        // have rotations & Pan/Dolly position variables inside to use with imGuIZMO.quat
                        // And it's necessary if you want use also direct-screen manipulator

mat4 compensateView; // compensate rotation of viewMatrix lookAt Matrix

void draw()
{
    glUseProgram(program);

    glBindVertexArray(vao);

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
    viewMatrix =  lookAt( { 12.0f,  6.0f,  4.0f },   // From / EyePos / PoV
                          {  0.0f,  0.0f,  0.0f },   // To   /  Tgt
                          {  3.0f,  1.0f,   .0f } ); // Up

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
    vec3 lightPos(2, 2.5, 3);        // Light Position

    // acquiring rotation for the light pos
    const float len = length(lightPos);
     //if(len<1.0 && len>= FLT_EPSILON) { normalize(lightPos); len = 1.0; }  // controls are not necessary: lightPos is known
     //else if(len > FLT_EPSILON)
        quat q = angleAxis(acosf(-lightPos.x/len), normalize(vec3(FLT_EPSILON, lightPos.z, -lightPos.y)));
    track.setSecondRot(q);          // store secondary rotation for the Light

    lightObj = translate(mat4(1), lightPos);
    lightObj = inverse(static_cast<mat4>(track.getSecondRot())) * lightObj ;

    setPerspective();
}

void glfwWindowSizeCallback(GLFWwindow* window, int w, int h)
{
    width = w; height = h;
    setPerspective();
    glViewport(0, 0, width, height);

    track.viewportSize(w, h);   // call it on resize window to re-adjust mouse sensitivity

    draw();
}

void glfwScrollCallback(GLFWwindow* window, double x, double y)
{
    track.wheel(x, y);
}

void initGL()
{

    readShaderFile vtx(STRING(APP_SHADERS_DIR) "/" VERT_NAME);
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vtx.buffer, NULL);
    glCompileShader(vertex);
    checkShader(vertex);

    readShaderFile frg(STRING(APP_SHADERS_DIR) "/" FRAG_NAME);
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &frg.buffer, NULL);
    glCompileShader(fragment);
    checkShader(fragment);

    program = glCreateProgram();

    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    checkProgram(program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    // cube data buffer

    glCreateVertexArrays(1, &vao);
    glCreateBuffers(1, &vaoBuffer);
    glNamedBufferStorage(vaoBuffer, sizeof(cubeData), cubeData, 0);

    glVertexArrayAttribBinding(vao,loc::vtxIdx, 0);
    glVertexArrayAttribFormat(vao, loc::vtxIdx, nElemVtx, GL_FLOAT, GL_FALSE, 0);
    glEnableVertexArrayAttrib(vao, loc::vtxIdx);

    glVertexArrayAttribBinding(vao,loc::nrmIdx, 0);
    glVertexArrayAttribFormat(vao, loc::nrmIdx, nElemVtx, GL_FLOAT, GL_FALSE, nElemVtx*sizeof(float));
    glEnableVertexArrayAttrib(vao, loc::nrmIdx);

    glVertexArrayAttribBinding(vao,loc::colIdx, 0);
    glVertexArrayAttribFormat(vao, loc::colIdx, nElemVtx, GL_FLOAT, GL_FALSE, 2*nElemVtx*sizeof(float));
    glEnableVertexArrayAttrib(vao, loc::colIdx);

    glVertexArrayVertexBuffer(vao, 0, vaoBuffer, 0, sizeof(cubeData[0]));

    ubo.create(sizeof(_uboMat), &uboMat, bind::matIdx );

    glViewport(0, 0, width, height);

    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glFrontFace(GL_CW);

    glDepthRange(-1.0, 1.0);
    setScene();
}

void initFramework()
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

/// vGizmo3D initialize: <br>
/// set/associate mouse BUTTON IDs and KEY modifier IDs to vGizmo3D functionalities <br><br>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void initVGizmo3D()     // Settings to control vGizmo3D
{
    // Initialization is necessary to associate your preferences to vGizmo3D
    // These are also the DEFAULT values, so if you want to maintain these combinations you can omit they
    // and to override only the associations that you want modify
        track.setGizmoRotControl         (vg::evButton1  /* or vg::evLeftButton */, 0 /* vg::evNoModifier */ );
    // Rotations around specific axis: mouse button and key modifier
        track.setGizmoRotXControl        (vg::evButton1  /* or vg::evLeftButton */, vg::evShiftModifier);
        track.setGizmoRotYControl        (vg::evButton1  /* or vg::evLeftButton */, vg::evControlModifier);
        track.setGizmoRotZControl        (vg::evButton1  /* or vg::evLeftButton */, vg::evAltModifier | vg::evSuperModifier);
    // Set vGizmo3D control for secondary rotation
        track.setGizmoSecondRotControl(vg::evButton2  /* or vg::evRightButton */, 0 /* vg::evNoModifier */ );
    // Pan and Dolly/Zoom: mouse button and key modifier
        track.setDollyControl            (vg::evButton2 /* or vg::evRightButton */, vg::evControlModifier);
        track.setPanControl              (vg::evButton2 /* or vg::evRightButton */, vg::evShiftModifier);
    // N.B. vg::enums are ONLY mnemonic: select and pass specific vg::enum to framework (that can have also different IDs)

    // passing the screen sizes calibrate drag rotation and auto-set the mouse sensitivity
        track.viewportSize(width, height);      // is necessary also to call when resize window/surface: re-calibrate drag rotation & auto-set mouse sensitivity
    // track.setGizmoFeeling(1.0);              // but if you need to more feeling with the mouse use: 1.0 default,  > 1.0 more sensible, < 1.0 less sensible

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
int getModifier(GLFWwindow* window) {
    if((glfwGetKey(window,GLFW_KEY_LEFT_CONTROL)    == GLFW_PRESS) || (glfwGetKey(window,GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS))
            return vg::evControlModifier;
    else if((glfwGetKey(window,GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) || (glfwGetKey(window,GLFW_KEY_RIGHT_SHIFT)   == GLFW_PRESS))
            return vg::evShiftModifier;
    else if((glfwGetKey(window,GLFW_KEY_LEFT_ALT)   == GLFW_PRESS) || (glfwGetKey(window,GLFW_KEY_RIGHT_ALT)     == GLFW_PRESS))
            return vg::evAltModifier;
    else if((glfwGetKey(window,GLFW_KEY_LEFT_SUPER) == GLFW_PRESS) || (glfwGetKey(window,GLFW_KEY_RIGHT_SUPER)   == GLFW_PRESS))
            return vg::evSuperModifier;
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

    // main render/draw loop
    while (!glfwWindowShouldClose(glfwWindow)) {
        glfwPollEvents();
        glClearBufferfv(GL_DEPTH, 0, &f);
        glClearBufferfv(GL_COLOR, 0, value_ptr(bgColor));

    // vGizmo3D: check changing button state to activate/deactivate drag movements (pressing together left/right activate/deactivate both)
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        static int leftPress = 0, rightPress = 0, middlePress = 0;
        double x, y;
        glfwGetCursorPos(glfwWindow, &x, &y);
        if(glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_LEFT) != leftPress) {   // check if leftButton state is changed
            leftPress = leftPress == GLFW_PRESS ? GLFW_RELEASE : GLFW_PRESS;        // set new (different!) state
            track.mouse(vg::evLeftButton, getModifier(glfwWindow),                  // send communication to vGizmo3D...
                                          leftPress, x, y);                         // ... checking if a key modifier currently is pressed
        }
        if(glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_RIGHT) != rightPress) { // same thing for rightButton
            rightPress = rightPress == GLFW_PRESS ? GLFW_RELEASE : GLFW_PRESS;
            track.mouse(vg::evRightButton, getModifier(glfwWindow),
                                           rightPress, x, y);
        }
        // Just a trik: simulating a double press (left+right button together) using MIDDLE button,
        // sending two "consecutive" activation/deactivation calls to rotate cube and light spot together
        if(glfwGetMouseButton(glfwWindow, GLFW_MOUSE_BUTTON_MIDDLE) != middlePress) {   // check if middleButton state is changed
            middlePress = middlePress == GLFW_PRESS ? GLFW_RELEASE : GLFW_PRESS;        // set new (different!) middle button state
            track.mouse(vg::evLeftButton, getModifier(glfwWindow),  middlePress, x, y); // call Left activation/deactivation with same "middleStatus"
            track.mouse(vg::evRightButton, getModifier(glfwWindow), middlePress, x, y); // call Right activation/deactivation with same "middleStatus"
        }
    // vGizmo3D: if "drag" active update internal rotations (primary and secondary)
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        track.motion(x,y);

    // vGizmo3D: call it every rendering loop if you want a continue rotation until you do not click on screen
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        track.idle();   // set continuous rotation on Idle: the slow rotation depends on speed of last mouse movement
                        // It can be adjusted from setIdleRotSpeed(1.0) > more speed, < less
                        // It can be stopped by click on screen (without mouse movement)
        track.idleSecond();


    // transferring the rotation to cube model matrix...
        mat4 modelMatrix = cubeObj * mat4_cast(track.getRotation());

    // Build a "translation" matrix
        mat4 translationMatrix = translate(mat4(1), track.getPosition());      // add translations (pan/dolly) to an identity matrix

    // old transformations used in "easy_examples": I comment and leave them to make less difficult the reading of the next steps
        //uboMat.mvpMatrix   = projMatrix * viewMatrix * compensateView * translationMatrix * modelMatrix  ;
        //uboMat.lightMatrix = projMatrix * viewMatrix * compensateView * translationMatrix * (static_cast<mat4>(vgTrackball.getSecondRot())) * lightObj;

        // Decomposition of the various transformations to use (in different way) with normal, vtx position and light
        uboMat.projMatrix  = projMatrix ;
        uboMat.viewMatrix  = viewMatrix ;
        uboMat.compMatrix  = compensateView;
        uboMat.modelMatrix = translationMatrix * modelMatrix;
        uboMat.lightMatrix = translationMatrix * static_cast<mat4>(track.getSecondRot()) * lightObj;

        // get PointOfView (camera position) from viewMatrix...
        uboMat.PoV = viewMatrix[3];    // in this example PoV does not change, so it would be useless to update it anytime...

        // some way to get light position:
        uboMat.lightPos = uboMat.lightMatrix * vec4(1);  // from LightMatrix
        // another way to get lightPos:
        // light has orbit invariant around cube, of ray always length(lightPos), so...
        // uboFrag.lightPos = getLightPosFromQuat(vgTrackball.refSecondRot(),length(lightPos)) + vgTrackball.getPosition();
        
        ubo.updateBufferData();

    // draw the cube, passing matrices to the vtx shader
        draw();

        glfwSwapBuffers(glfwWindow);
    }

    // Cleanup OpenGL
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vaoBuffer);
    glDeleteProgram(program);

    // Cleanup Framework
    glfwDestroyWindow(glfwWindow);
    glfwTerminate();

    return EXIT_SUCCESS;
}