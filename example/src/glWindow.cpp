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
#include "qJulia.h"

#include "glWindow.h"

///////////////////////////////////////////////////////////////////////////////
// UNCOMMENT for screen manipulato virtualGizmo3D 
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// if you want use also the virtualGizmo3D, screen manipulator, uncomment this:


// Global variable or member class
#ifdef GLAPP_USE_VIRTUALGIZMO // Enable it in glWindows.h or with compiler define
    
    vg::vGizmo3D gizmo; 
    vg::vGizmo3D &getGizmo() { return gizmo; }

    void setRotation(const quat &q) { getGizmo().setRotation(q); }
    quat getRotation() { return getGizmo().getRotation(); }

    void setPosition(const vec3 &p) { getGizmo().setPosition(p); }
    vec3 getPosition() { return getGizmo().getPosition(); }
#else
/////////////////////////////////////////////////////////////////////////////
// For imGuIZMO, declare global variable or member class quaternion
    quat qRot = quat(1.f, 0.f, 0.f, 0.f);
    vec3 position = vec3(0.f);

/////////////////////////////////////////////////////////////////////////////
// two helper functions, not really necessary (but comfortable)
    void setRotation(const quat &q) { qRot = q; }
    quat const &getRotation() { return qRot; }
    void setPosition(const vec3 &p) { position = p; }
    vec3 const &getPosition() { return position; }
#endif

//using namespace glm;

void setView()
{

    vec3 povVec( 0.f, 0.f, 7.f);
    vec3 tgtVec( 0.f, 0.f, 0.f);


    theWnd->projectionMatrix = perspective(radians(30.0f),(float)theApp->GetWidth()/(float)theApp->GetHeight(),0.0f,30.0f);
    theWnd->modelMatrix      = mat4(1.0f);
    theWnd->viewMatrix       = lookAt( povVec,
                                       tgtVec,
                                       vec3(0.0f, 1.0f, 0.0f));
#ifdef GLAPP_USE_VIRTUALGIZMO 
    getGizmo().applyTransform(theWnd->modelMatrix);
#endif

    theWnd->mvMatrix         = theWnd->viewMatrix * theWnd->modelMatrix;
    theWnd->mvpMatrix        = theWnd->projectionMatrix * theWnd->mvMatrix;
   
}


void setViewOrtho()
{

    theWnd->projectionMatrix = ortho(-1.0f, 1.0f, -1.0f, 1.0f, -2.0f, 2.0f);
    theWnd->modelMatrix      = mat4(1.0f);
    theWnd->viewMatrix       = lookAt( vec3(0.f, 0.f, 1.f),
                                       vec3(0.f, 0.f, 0.f),
                                       vec3(0.f, 1.f, 0.f));

    theWnd->mvpMatrix        = theWnd->projectionMatrix * theWnd->viewMatrix * theWnd->modelMatrix;

}



void glWindow::Create(const char *wTitle, int w, int h, int xPos, int yPos)
{

}

glWindow::glWindow()
{
    

}


glWindow::~glWindow()
{

}



//
/////////////////////////////////////////////////
void glWindow::onInit()
{

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);				// Black Background

    glViewport(0,0,theApp->GetWidth(), theApp->GetHeight());
    //setViewOrtho();
    //setView();

#ifdef GLAPP_USE_VIRTUALGIZMO 
#ifdef GLAPP_USE_SDL
    getGizmo().setGizmoRotControl( (vgButtons) SDL_BUTTON_LEFT, (vgModifiers) 0 /* evNoModifier */ );

    getGizmo().setGizmoRotXControl((vgButtons) SDL_BUTTON_LEFT, (vgModifiers) KMOD_SHIFT);
    getGizmo().setGizmoRotYControl((vgButtons) SDL_BUTTON_LEFT, (vgModifiers) KMOD_CTRL);
    getGizmo().setGizmoRotZControl((vgButtons) SDL_BUTTON_LEFT, (vgModifiers) KMOD_ALT);

    getGizmo().setDollyControl((vgButtons) SDL_BUTTON_RIGHT, (vgModifiers) 0);
    getGizmo().setPanControl(  (vgButtons) SDL_BUTTON_RIGHT, (vgModifiers) KMOD_CTRL|KMOD_SHIFT);
#else
    getGizmo().setGizmoRotControl( (vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) 0 /* evNoModifier */ );

    getGizmo().setGizmoRotXControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) GLFW_MOD_SHIFT);
    getGizmo().setGizmoRotYControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) GLFW_MOD_CONTROL);
    getGizmo().setGizmoRotZControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) GLFW_MOD_ALT | GLFW_MOD_SUPER);

    getGizmo().setDollyControl((vgButtons) GLFW_MOUSE_BUTTON_RIGHT, (vgModifiers) 0);
    getGizmo().setPanControl(  (vgButtons) GLFW_MOUSE_BUTTON_RIGHT, (vgModifiers) GLFW_MOD_CONTROL|GLFW_MOD_SHIFT);
#endif
    //getGizmo().setPanControls(  (vgButtons) GLFW_MOUSE_BUTTON_RIGHT, (vgModifiers) GLFW_MOD_SHIFT);

    // viewportSize  is need to set mouse sensitivity for rotation
    // You nedd to call it in your "reshape" function: when resize the window
    // look below
    getGizmo().viewportSize(theApp->GetWidth(), theApp->GetHeight());

    // If you need to more feeling with the mouse use:
    // getGizmo().setGizmoFeeling(1.0);
    // 1.0 default,  > 1.0 more sensible, < 1.0 less sensible
    float scale = 1.f/(theApp->GetWidth() > theApp->GetHeight() ? theApp->GetHeight() : theApp->GetWidth());
    getGizmo().setDollyScale(scale);
    getGizmo().setPanScale(scale);
    

    // other settings if you need it
    /////////////////////////////////////////////

    getGizmo().setDollyPosition(0.0f);
    //getGizmo().setRotationCenter(vec3(0.0));
#endif
    qjSet = new qJulia;

}



//
/////////////////////////////////////////////////
void glWindow::onExit()
{
    delete qjSet;
    
}

//
/////////////////////////////////////////////////
void glWindow::onRender()
{
    glViewport(0,0,theApp->GetWidth(), theApp->GetHeight());
    glClear(GL_COLOR_BUFFER_BIT);

#ifdef GLAPP_USE_VIRTUALGIZMO 
    qjSet->matOrientation = getGizmo().getTransform();
    qjSet->position = getGizmo().getPosition();

#else
////////////////////////////////////////////////////////////////////
// imGuIZMO: get quaternion in to orientation Matrix

#ifndef IMGUIZMO_USE_ONLY_ROT
    //qjSet->matOrientation = transfMat * (1.0+position.z);
    //mat4 modelMatrix = mat4_cast(qRot);
    //mat4 m(1.f); translate(m, vec3(position.x, position.y, position.z));
    mat4 modelMatrix = mat4_cast(qRot);
    qjSet->matOrientation = modelMatrix;
    qjSet->position = position;
#else
    mat4 modelMatrix = mat4_cast(qRot);
    qjSet->matOrientation = modelMatrix;
#endif

#endif
    qjSet->render(); 
}



//
/////////////////////////////////////////////////
void glWindow::onIdle()
{
#ifdef GLAPP_USE_VIRTUALGIZMO 
    getGizmo().idle();
#endif
}


//
/////////////////////////////////////////////////
void glWindow::onReshape(GLint w, GLint h)
{
    glViewport(0,0,w,h);
    theApp->SetWidth(w); theApp->SetHeight(h);

#ifdef GLAPP_USE_VIRTUALGIZMO 
    getGizmo().viewportSize(w, h);
#endif
}

//
/////////////////////////////////////////////////
void glWindow::onKeyUp(unsigned char key, int x, int y)
{

}


//
/////////////////////////////////////////////////
void glWindow::onSpecialKeyDown(int key, int x, int y)
{


}


//
/////////////////////////////////////////////////
void glWindow::onKeyDown(unsigned char key, int x, int y)
{



}



//
/////////////////////////////////////////////////
void glWindow::onSpecialKeyUp(int key, int x, int y)
{



}


//
/////////////////////////////////////////////////
void glWindow::onMouseButton(int button, int upOrDown, int x, int y)
{

#ifdef GLAPP_USE_VIRTUALGIZMO 
    getGizmo().mouse((vgButtons) (button),
                    (vgModifiers) theApp->getModifier(),
#ifdef GLAPP_USE_SDL
                    upOrDown==SDL_MOUSEBUTTONDOWN, x, y);
#else
                    upOrDown==GLFW_PRESS, x, y);
#endif
#endif

}

//
/////////////////////////////////////////////////
void glWindow::onMouseWheel(int wheel, int direction, int x, int y)
{

}

//
/////////////////////////////////////////////////
void glWindow::onMotion(int x, int y)
{
#ifdef GLAPP_USE_VIRTUALGIZMO 
    getGizmo().motion(x, y);
#endif

    //qjSet->matOrientation *= trackball.getDollyPosition();
    
}

//
/////////////////////////////////////////////////
void glWindow::onPassiveMotion(int x, int y)
{

}
