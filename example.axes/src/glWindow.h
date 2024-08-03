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

#include "glApp.h"
#include "tools/vGizmo.h"


/*
///////////////////////////////////////////
//Data Init for 32/64 bit systems
//////////////////////////////////////////

template<int> void IntDataHelper();

template<> void IntDataHelper<4>() 
{
  // do 32-bits operations
}

template<> void IntDataHelper<8>() 
{
  // do 64-bits operations
}

// helper function just to hide clumsy syntax
inline void IntData() { IntDataHelper<sizeof(size_t)>(); }
*/

#include <cstdint>
#if INTPTR_MAX == INT32_MAX 
    //APP compiling 32bit    
#elif INTPTR_MAX == INT64_MAX
    //APP compiling 32bit
#else
    #error "Environment not 32 or 64-bit."
#endif




using namespace std;

class glApp;
class oglAxes;



typedef float tbT;

class glWindow /*: public glApp*/
{
    mainGLApp *GetFrame()  { return theApp; }
    mainGLApp *GetCanvas() { return theApp; }   

public:

    glWindow();
    virtual ~glWindow();


    // Called when Window is created
    virtual void onInit();
    // Called when Window is closed.
    virtual void onExit();

    // The onIdle and onDisplay methods should also be overloaded.
    // Within the onIdle method put the logic of the application.
    // The onDisplay method is for any drawing code.
    virtual void onIdle();
    virtual void onRender();
    virtual void onReshape(GLint w, GLint h);

    virtual void onMouseButton(int button, int upOrDown, int x, int y);
    virtual void onMouseWheel(int wheel, int direction, int x, int y);
    virtual void onMotion(int x, int y);
    virtual void onPassiveMotion(int x, int y);

    // The onKeyDown method handles keyboard input that are standard ASCII keys
    virtual void onKeyDown(unsigned char key, int x, int y);
    virtual void onKeyUp(unsigned char key, int x, int y);
    virtual void onSpecialKeyUp(int key, int x, int y);
    virtual void onSpecialKeyDown(int key, int x, int y);


    int GetWidth()  { return GetCanvas()->GetWidth();  }
    int GetHeight() { return GetCanvas()->GetHeight(); }

    oglAxes *getAxes() { return axes; }

private:
    mat3 matOrientation = mat3(1.0f);
    vec3 position = vec3(0.f);


    oglAxes *axes;

};
