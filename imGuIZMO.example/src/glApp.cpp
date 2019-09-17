///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2018 Michele Morrone
//  All rights reserved.
//
//  mailto:me@michelemorrone.eu
//  mailto:brutpitt@gmail.com
//  
//  https://github.com/BrutPitt
//
//  https://michelemorrone.eu
//  
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the <organization> nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//  
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////
#include <chrono>
#include <array>
#include <vector>

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #include <emscripten/html5.h>
    #include <GLES3/gl3.h>
#else
    #include "libs/glad/glad.h"
#endif

#include "glApp.h"
#include "glWindow.h"

//#include "libs/ImGui/imgui.h"
//#include "libs/ImGui/imgui_impl_glfw_gl3.h"


#ifndef __EMSCRIPTEN__
    GLFWmonitor* getCurrentMonitor(GLFWwindow *window);
    void toggleFullscreenOnOff(GLFWwindow* window);
    bool isDoubleClick(int button, int action, double x, double y, double ms);
#endif


// Set the application to null for the linker
mainGLApp* mainGLApp::theMainApp = 0;

bool        ImGui_ImplGlfwGL3_Init(GLFWwindow* , bool , const char* );

static void glfwErrorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //if ((key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL) && action == GLFW_PRESS)
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        theDlg.visible(theDlg.visible()^1);
    } //glfwSetWindowShouldClose(window,GLFW_TRUE);
     
    if(key == GLFW_KEY_PRINT_SCREEN && action == GLFW_PRESS) {
        if(scancode & GLFW_MOD_CONTROL) {} // CTRL+PrtScr -> request FileName
        if(!scancode)                   {} // CTRL+PrtScr -> TimeBased FileName
    }
    else if(key >= GLFW_KEY_F1 && key <= GLFW_KEY_F12 && action == GLFW_PRESS) {
        theWnd->onSpecialKeyDown(key, 0, 0);
#ifndef __EMSCRIPTEN__
        if(key == GLFW_KEY_F11) toggleFullscreenOnOff(window);
#endif

    } else if( key == GLFW_KEY_SPACE && action == GLFW_PRESS) { } //SPACE
    else if(action == GLFW_PRESS) {            
        theWnd->onKeyDown(key==GLFW_KEY_ENTER ? 13 : key, 0, 0);
    }
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    if(ImGui::GetIO().WantCaptureKeyboard) return;

}



void glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{   
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    if(ImGui::GetIO().WantCaptureMouse)  return;


    double x,y;
    glfwGetCursorPos(window, &x, &y);

    if (action == GLFW_PRESS) {
#ifndef __EMSCRIPTEN__
        if(isDoubleClick(button, action, x , y, 300) ) toggleFullscreenOnOff(window);
        else 
#endif            
            theWnd->onMouseButton(button, GLFW_PRESS, x, y); 
        //getApp()->LeftButtonDown();
            
    } else if (action == GLFW_RELEASE) {
#ifndef __EMSCRIPTEN__
        isDoubleClick(button, action, x , y, 300);
#endif            
        theWnd->onMouseButton(button, GLFW_RELEASE, x, y); 
        
        //getApp()->LeftButtonUp();
    }


}


static void glfwCharCallback(GLFWwindow* window, unsigned int c)
{
    ImGui_ImplGlfw_CharCallback(window, c);
    if(ImGui::GetIO().WantCaptureKeyboard) return;
}

void glfwScrollCallback(GLFWwindow* window, double x, double y)
{
    ImGui_ImplGlfw_ScrollCallback(window, x, y);
    if(ImGui::GetIO().WantCaptureMouse) return;
}


static void glfwMousePosCallback(GLFWwindow* window, double x, double y)
{
        //ImGui::GetIO().MousePos.x = (float) x;
        //ImGui::GetIO().MousePos.y = (float) y;

    if((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)  == GLFW_PRESS) || 
       (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) )
        theWnd->onMotion(x, y); 
}


void glfwWindowSizeCallback(GLFWwindow* window, int width, int height)
{

    theWnd->onReshape(width,height);
}


#ifndef __EMSCRIPTEN__

bool isDoubleClick(int button, int action, double x, double y, double ms)
{
    static auto before = std::chrono::system_clock::now();
    static int oldAction = -1;
    static int oldButton = -1;
    static double oldx=-1, oldy=-1;    

    auto now = std::chrono::system_clock::now();
    double diff_ms = std::chrono::duration <double, std::milli> (now - before).count();

    bool retval = (diff_ms<ms && (oldx==x && oldy==y) && 
                  (oldButton == button && button == GLFW_MOUSE_BUTTON_LEFT) && 
                  (action==GLFW_PRESS && oldAction==GLFW_RELEASE));
        
    before = now;    
    oldAction=action;
    oldButton = button;
    oldx=x, oldy=y;

    return retval;

}

void toggleFullscreenOnOff(GLFWwindow* window)
{
    static int windowed_xpos, windowed_ypos, windowed_width, windowed_height;
                
    if (glfwGetWindowMonitor(window))
    {
        glfwSetWindowMonitor(window, NULL,
                                windowed_xpos, windowed_ypos,
                                windowed_width, windowed_height, 0);
    }
    else
    {
        GLFWmonitor* monitor = getCurrentMonitor(window);
        if (monitor)
        {
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            glfwGetWindowPos(window, &windowed_xpos, &windowed_ypos);
            glfwGetWindowSize(window, &windowed_width, &windowed_height);
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        }
    }

}

GLFWmonitor* getCurrentMonitor(GLFWwindow *window)
{
    int nmonitors, i;
    int wx, wy, ww, wh;
    int mx, my, mw, mh;
    int overlap, bestoverlap;
    GLFWmonitor *bestmonitor;
    GLFWmonitor **monitors;
    const GLFWvidmode *mode;

    bestoverlap = 0;
    bestmonitor = NULL;

    glfwGetWindowPos(window, &wx, &wy);
    glfwGetWindowSize(window, &ww, &wh);
    monitors = glfwGetMonitors(&nmonitors);

    for (i = 0; i < nmonitors; i++) {
        mode = glfwGetVideoMode(monitors[i]);
        glfwGetMonitorPos(monitors[i], &mx, &my);
        mw = mode->width;
        mh = mode->height;

        overlap =
            std::max(0, std::min(wx + ww, mx + mw) - std::max(wx, mx)) *
            std::max(0, std::min(wy + wh, my + mh) - std::max(wy, my));

        if (bestoverlap < overlap) {
            bestoverlap = overlap;
            bestmonitor = monitors[i];
        }
    }

    return bestmonitor;
}
#endif

#ifdef __EMSCRIPTEN__
    #define USE_GLSL_VERSION   "#version 300 es\n\n"
    #define USE_GLSL_PRECISION "precision highp float;\n"
#else
    #define USE_GLSL_VERSION   "#version 410"
    #define USE_GLSL_PRECISION 

#endif

// ImGui utils
/////////////////////////////////////////////////
void mainGLApp::imguiInit()
{
    // Setup ImGui binding
        ImGui::CreateContext();
        //ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui_ImplGlfw_InitForOpenGL(mainGLFWwnd, false);
        ImGui_ImplOpenGL3_Init(USE_GLSL_VERSION);
        //ImGui::StyleColorsDark();
        setGUIStyle();

}

#undef USE_GLSL_VERSION  
#undef USE_GLSL_PRECISION

int mainGLApp::imguiExit()
{
// need to test exit wx ... now 0!
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    return 0;
}

GLFWwindow *secondary;

// glfw utils
/////////////////////////////////////////////////
void mainGLApp::glfwInit()
{
    glfwSetErrorCallback(glfwErrorCallback);

    if (!::glfwInit()) exit(EXIT_FAILURE);
#ifdef __EMSCRIPTEN__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    //glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API) ;
#else       
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    #ifdef GLAPP_REQUIRE_OGL45
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    #else
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    #endif
#endif    
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
    

    setGLFWWnd(glfwCreateWindow(GetWidth(), GetHeight(), getWindowTitle(), NULL, NULL));
    if (!getGLFWWnd())
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    //secondary = glfwCreateWindow(512, 512, "My Engine", NULL, getGLFWWnd());

    glfwMakeContextCurrent(getGLFWWnd());

    //glfwSetWindowOpacity(getGLFWWnd(),.5);

#ifdef __EMSCRIPTEN__
/*
    emscripten_set_mousedown_callback("#canvas", nullptr, true, 
        [](int, const EmscriptenMouseEvent* e, void*)->EMSCRIPTEN_RESULT {
            if ((e->button >= 0) && (e->button < 3)) {
                ImGui::GetIO().MouseDown[e->button] = true;
            }
            return true;
        });
    emscripten_set_mouseup_callback("#canvas", nullptr, true, 
        [](int, const EmscriptenMouseEvent* e, void*)->EMSCRIPTEN_RESULT {
            if ((e->button >= 0) && (e->button < 3)) {
                ImGui::GetIO().MouseDown[e->button] = false;
            }
            return true;
        });
*/
#else
    //Init OpenGL
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
#endif



    glfwSetKeyCallback(getGLFWWnd(), glfwKeyCallback);
    glfwSetCharCallback(getGLFWWnd(), glfwCharCallback);
    glfwSetMouseButtonCallback(getGLFWWnd(), glfwMouseButtonCallback);
    glfwSetScrollCallback(getGLFWWnd(), glfwScrollCallback);
    glfwSetCursorPosCallback(getGLFWWnd(), glfwMousePosCallback);
    glfwSetWindowSizeCallback(getGLFWWnd(), glfwWindowSizeCallback);

    glfwSwapInterval(1);

}

int mainGLApp::glfwExit()
{

    glfwDestroyWindow(getGLFWWnd());
    glfwTerminate();

// need to test exit glfw ... now 0!
    return 0;

}

int mainGLApp::getModifier() {
    GLFWwindow* window = getGLFWWnd();
    if((glfwGetKey(window,GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) || (glfwGetKey(window,GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS))
            return GLFW_MOD_CONTROL;
    else if((glfwGetKey(window,GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) || (glfwGetKey(window,GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
            return GLFW_MOD_SHIFT;
    else if((glfwGetKey(window,GLFW_KEY_LEFT_ALT) == GLFW_PRESS) || (glfwGetKey(window,GLFW_KEY_RIGHT_ALT) == GLFW_PRESS))
            return GLFW_MOD_ALT;
    else return 0;
}


mainGLApp::mainGLApp() 
{    
    // Allocation in main(...)
    mainGLApp::theMainApp = this;
    glEngineWnd = new glWindow; 
}

mainGLApp::~mainGLApp() 
{
    onExit();

    delete glEngineWnd;
}

void mainGLApp::onInit() 
{

    xPosition = yPosition = -1;
    width = 1280; height = 800;
    windowTitle = "quaternion Julia set";

//Init OpenGL


// Imitialize both FrameWorks
    glfwInit();

// Imitialize both GL engine
    glEngineWnd->onInit();

    imguiInit();

}

int mainGLApp::onExit()  
{
    glEngineWnd->onExit();
// Exit from both FrameWorks
    imguiExit();

    glfwExit();

// need to test returns code... now 0!        
    return 0;
}


void newFrame()
{
#ifdef __EMSCRIPTEN__
    glfwPollEvents();
#endif

    theWnd->onIdle();
    theWnd->onRender();

    // Rendering
//        int display_w, display_h;
    //glViewport(0, 0, theApp->GetWidth(), theApp->GetHeight());
    //glClearColor(1,0,0,0);
    //glClear(GL_COLOR_BUFFER_BIT);

    theDlg.renderImGui();
    glfwMakeContextCurrent(theApp->getGLFWWnd());
    glfwSwapBuffers(theApp->getGLFWWnd());

}


void mainGLApp::mainLoop() 
{
    while (!glfwWindowShouldClose(getGLFWWnd())) {          

        glfwPollEvents();
        newFrame();
/*
        if (!glfwGetWindowAttrib(getGLFWWnd(), GLFW_ICONIFIED)) {

        }
*/
/*
        static int cnt=0;
        glfwMakeContextCurrent(secondary);

        glClearColor(0.0, 0.0, 0.0, 0.1);
        glClear(GL_COLOR_BUFFER_BIT);

        
        glfwSwapBuffers(secondary);

        glfwMakeContextCurrent(getGLFWWnd());
*/

        //if(!(cnt++%10)) 


    }

}
    



/////////////////////////////////////////////////
// classic entry point
int main(int argc, char **argv)
{
//Initialize class e self pointer
    theApp = new mainGLApp;    

    theApp->onInit();

/////////////////////////////////////////////////
// Enter in GL main loop
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(newFrame,0,true);
#else
    theApp->mainLoop();
#endif

/////////////////////////////////////////////////
// Exit procedures called from theApp destructor
   
    return 0;

}