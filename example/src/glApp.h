//------------------------------------------------------------------------------
//  Copyright (c) 2018-2019 Michele Morrone
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

#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <iostream>

#if !defined(__EMSCRIPTEN__)
    //#define GLAPP_USE_SDL
#endif

#ifdef GLAPP_USE_SDL
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_opengl.h>
#else
    #include <GLFW/glfw3.h>
#endif

#include "ui/uiMainDlg.h"



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
////////////////////////////////
//GLFW Utils

    glWindow *getEngineWnd() { return glEngineWnd; }
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



