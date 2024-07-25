//------------------------------------------------------------------------------
//  Copyright (c) 2018-2024 Michele Morrone
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
#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <iostream>

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #include <emscripten/html5.h>
    #include <GLES3/gl3.h>
#else
    #include "libs/glad/glad.h"
#endif

#ifdef GLAPP_USE_SDL
    #include <SDL.h>
    #include <SDL_opengl.h>
#else
    #include <GLFW/glfw3.h>
#endif

#include "ui/uiMainDlg.h"


#ifdef GLAPP_USE_IMGUI
#include "ui\uiMainDlg.h"
#endif


#ifndef theApp
    #define theApp mainGLApp::theMainApp
#endif
#ifndef theWnd
    #define theWnd theApp->getEngineWnd()
#endif
#ifndef theDlg
    #define theDlg theApp->getMainDlg()
#endif



enum ScreeShotReq {
    ScrnSht_NO_REQUEST,
    ScrnSht_SILENT_MODE,
    ScrnSht_FILE_NAME
};


class glWindow;

void setGUIStyle();

/////////////////////////////////////////////////
// theApp -> Main App -> container
/////////////////////////////////////////////////

class mainGLApp
{
public:
    // self pointer .. static -> the only one 
    static mainGLApp* theMainApp;

    mainGLApp();
    ~mainGLApp();

    void onInit();
    int onExit();

    void mainLoop();
    glWindow *getEngineWnd() { return glEngineWnd; }

////////////////////////////////
// SDL / GLFW Windows
#ifdef GLAPP_USE_SDL
    SDL_Window* getSDLWWnd()  const { return(mainSDLWwnd);  }
    void setSDLWWnd(SDL_Window* wnd) { mainSDLWwnd = wnd; }
#else
    GLFWwindow* getGLFWWnd()  const { return(mainGLFWwnd);  }
    void setGLFWWnd(GLFWwindow* wnd) { mainGLFWwnd = wnd; }
#endif

	int getXPosition() const { return(xPosition); }
    int getYPosition() const { return(yPosition); }
	int GetWidth()     const { return(width);     }
	int GetHeight()    const { return(height);    }
    void SetWidth(int v)  { width  = v; }
    void SetHeight(int v) { height = v; }
	const char* getWindowTitle() const { return(windowTitle.c_str()); }

protected:

		// The Position of the window
		int xPosition, yPosition;
		int width, height;
		/** The title of the window */

        bool exitFullScreen;

		// The title of the window
		std::string windowTitle;
    
    
private:
// imGui utils
/////////////////////////////////////////////////
    void imguiInit();
    int imguiExit();

    mainImGuiDlgClass mainImGuiDlg;
public:
    mainImGuiDlgClass &getMainDlg() { return mainImGuiDlg; }


private:

// SDL / GLFW utils
/////////////////////////////////////////////////
#ifdef GLAPP_USE_SDL
    void frameInit();
    int frameExit();
    SDL_Window* mainSDLWwnd;
    SDL_GLContext gl_context;
#else
    void frameInit();
    int frameExit();
    GLFWwindow* mainGLFWwnd;
#endif
    int getModifier();

    glWindow *glEngineWnd;

friend class glWindow;

};



