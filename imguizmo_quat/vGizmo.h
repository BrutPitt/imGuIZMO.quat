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
#pragma once

#define VGIZMO_H_FILE
#include "vgMath.h"

#ifdef VGM_USES_TEMPLATE
    #define VGIZMO_BASE_CLASS virtualGizmoBaseClass<T>
    #define imGuIZMO_BASE_CLASS virtualGizmoBaseClass<float>
#else
    #define VGIZMO_BASE_CLASS virtualGizmoBaseClass
    #define imGuIZMO_BASE_CLASS VGIZMO_BASE_CLASS
    #define T VG_T_TYPE
#endif



typedef int vgButtons;
typedef int vgModifiers;

namespace vg {
//  Default values for button and modifiers.
//      This values are aligned with GLFW defines (for my comfort),
//      but they are loose from any platform library: simply initialize
//      the virtualGizmo with your values: 
//          look at "onInit" in glWindow.cpp example.
//--------------------------------------------------------------------
    enum {
        evLeftButton  ,
        evRightButton ,
        evMiddleButton
    };

    enum {
        evButton1 ,
        evButton2 ,
        evButton3 ,
        evButton4 ,
        evButton5 ,
        evButton6 ,
        evButton7 ,
        evButton8 ,
        evButton9 
    };

    enum {
        evNoModifier      =  0,
        evShiftModifier   =  1   ,
        evControlModifier =  1<<1,
        evAltModifier     =  1<<2,
        evSuperModifier   =  1<<3  
    };

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//
//  Base manipulator class
//
//--------------------------------------------------------------------
//--------------------------------------------------------------------
TEMPLATE_TYPENAME_T class virtualGizmoBaseClass {

public:
    virtualGizmoBaseClass() : tbActive(false), pos(0), delta(0),
                           tbControlButton(evLeftButton), tbControlModifiers(evNoModifier),
                           tbRotationButton(evRightButton), xRotationModifier(evShiftModifier),
                           yRotationModifier(evControlModifier),  zRotationModifier(evAltModifier|evSuperModifier)
    { 
        viewportSize(T(256), T(256));  //initial dummy value
    }
    virtual ~virtualGizmoBaseClass() {}

    //    Call to initialize and on reshape
    //--------------------------------------------------------------------------
/// Adjoust mouse sensitivity in base to viewport dimensions
///@param[in]  w T : current WIDTH  of window/viewport/screen
///@param[in]  h T : current HEIGHT of window/viewport/screen
///@code
///    vg::vGizmo3D track;
///
///    // call on initialization and on window/viewport resize
///    track.viewportSize(width, height);
///@endcode
    void viewportSize(T w, T h) {
        width = w; height = h; 
        minVal = T(width < height ? width*T(0.5) : height*T(0.5));
        offset = tVec3(T(.5 * width), T(.5 * height), T(0));
    }

    void inline activateMouse(T x, T y) {
        pos.x = x;
        pos.y = y;
        delta.x = delta.y = 0;
    }
    void inline deactivateMouse() {
        if(delta.x == 0 && delta.y == 0) update();
        delta.x = delta.y = 0;
    }
    void inline testRotModifier(int x, int y, vgModifiers mod) { }
    
/// Start/End mouse capture: call on mouse BUTTON event or on state change
///@param[in]  b enum vgButtons : button pressed/released (BUTTON ID)
///@param[in]  m enum vgModifiers : current KEY modifier ID (if active) or evNoModifier = 0
///@param[in]  pressed bool : mouse button pressed = true, released = false
///@param[in]  x T : current X screen coord of mouse cursor
///@param[in]  y T : current Y screen coord of mouse cursor
///@code
///    vg::vGizmo3D track;
///
///    // call on mouse BUTTON event or check in main render loop
///    track.mouse((vgButtons) button, (vgModifiers) modifier, pressed, x, y);
///@endcode
    virtual void mouse( vgButtons button, vgModifiers mod, bool pressed, T x, T y)
    {
        if ( (button == tbControlButton) && pressed && (tbControlModifiers ? tbControlModifiers & mod : tbControlModifiers == mod) ) {
            tbActive = true;
            activateMouse(x,y);
        }
        else if ( button == tbControlButton && !pressed) {
            deactivateMouse();
            tbActive = false;
        }

        if((button == tbRotationButton) && pressed) {
            if      (xRotationModifier & mod) { tbActive = true; rotationVector = tVec3(T(1), T(0), T(0)); activateMouse(x,y); }
            else if (yRotationModifier & mod) { tbActive = true; rotationVector = tVec3(T(0), T(1), T(0)); activateMouse(x,y); }
            else if (zRotationModifier & mod) { tbActive = true; rotationVector = tVec3(T(0), T(0), T(1)); activateMouse(x,y); }
        } else if((button == tbRotationButton) && !pressed) { 
            deactivateMouse(); rotationVector = tVec3(T(1)); tbActive = false;
        }
    
    }

/// Update rotations/positions in base to mouse movements: call on mouse MOTION event
///@param[in]  x T : current X screen coord of mouse cursor
///@param[in]  y T : current Y screen coord of mouse cursor
///@code
///    vg::vGizmo3D track;
///
///    // call on mouse MOTION event or check in main render loop
///    if((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)  == GLFW_PRESS) ||
///       (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) )
///        track.motion((int)x, (int)y);
///@endcode
    virtual void motion( T x, T y) {
        delta.x = x - pos.x;   delta.y = pos.y - y;
        pos.x = x;   pos.y = y;
        update();
    }
    //    Call on Pinching
    //--------------------------------------------------------------------------
    void pinching(T d, T z = T(0)) {
        delta.y = d * z;
        update();
    }

/// <b>Call in main render loop to implement a continue slow rotation </b><br>
/// <br>
/// This rotation depends on speed of last mouse movements and maintains same spin <br>
/// The speed can be adjusted from <b>setIdleRotSpeed(1.0)</b>
/// It can be stopped by click on screen (without mouse movement)
///@code
///     while (!glfwWindowShouldClose(glfwWindow)) {
///         ...
///         track.idle();       // set continuous rotation on Idle
///@endcode
    void idle() { qtRot = qtIdle*qtRot;  }

    //    Call after changed settings
    //--------------------------------------------------------------------------
    virtual void update() = 0;
    void updateGizmo()
    {
        if(!delta.x && !delta.y) {
            qtIdle = qtStep = tQuat(T(1), T(0), T(0), T(0)); //no rotation
            return;
        }

        tVec3 a(T(pos.x-delta.x), T(height - (pos.y+delta.y)), T(0));
        tVec3 b(T(pos.x    ),     T(height -  pos.y         ), T(0));

        auto vecFromPos = [&] (tVec3 &v) {
            v -= offset;
            v /= minVal;
            const T len = length(v);
            v.z = len>T(0) ? pow(T(2), -T(.5) * len) : T(1);
            v = normalize(v);
        };

        vecFromPos(a);
        vecFromPos(b);

        tVec3 axis = cross(a, b);
        axis = normalize(axis);

        T AdotB = dot(a, b);
        T angle = acos( AdotB>T(1) ? T(1) : (AdotB<-T(1) ? -T(1) : AdotB)); // clamp need!!! corss float is approximate to FLT_EPSILON

        qtStep = normalize(angleAxis(angle * tbScale * fpsRatio                  , axis * rotationVector));
        qtIdle = normalize(angleAxis(angle * tbScale * fpsRatio * qIdleSpeedRatio, axis * rotationVector));
        qtRot = qtStep*qtRot;

    }

///  Set the mouse sensitivity for vGizmo3D
///@param[in]  scale float : values > 1.0 more, values < 1.0 less
    void setGizmoFeeling( T scale) { tbScale = scale; }
    //  Call with current fps (every rendering) to adjust "auto" sensitivity
    //////////////////////////////////////////////////////////////////
    void setGizmoFPS(T fps) { fpsRatio = T(60.0)/fps;}

    //  Apply rotation
    //////////////////////////////////////////////////////////////////
    inline void applyRotation(tMat4 &m) { m = m * mat4_cast(qtRot); }                                     

    //  Set the point around which the virtualGizmo will rotate.
    //////////////////////////////////////////////////////////////////
    void setRotationCenter( const tVec3& c) { rotationCenter = c; }
    tVec3& getRotationCenter() { return rotationCenter; }

///  Set mouse BUTTON and KEY modifier for main rotation
///@param[in]  b enum vgButtons : associate your / framework (GLFW/SDL/WIN32/etc)
///                 mouse BUTTON ID
///@param[in]  m enum vgModifiers : associate your / framework (GLFW/SDL/WIN32/etc)
///                 KEY modifier ID : CTRL / ALT / SUPER / SHIFT
///@code
/// static vg::vGizmo3D track;
/// ...
///   // Initialize main rotation
///   track.setGizmoRotControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) 0 /* evNoModifier */ );
///
///   // Rotations around specific axis: mouse button and key modifier
///   t.setGizmoRotXControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT,  (vgModifiers) GLFW_MOD_SHIFT);
///   t.setGizmoRotYControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT,  (vgModifiers) GLFW_MOD_CONTROL);
///   t.setGizmoRotZControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT,  (vgModifiers) GLFW_MOD_ALT);
///
///   // Pan and Dolly/Zoom: mouse button and key modifier
///   t.setDollyControl((vgButtons) GLFW_MOUSE_BUTTON_RIGHT, (vgModifiers) 0 /* evNoModifier */);
///   t.setPanControl((vgButtons) GLFW_MOUSE_BUTTON_RIGHT, (vgModifiers) GLFW_MOD_ALT | GLFW_MOD_SHIFT);
///@endcode
    void setGizmoRotControl( vgButtons b, vgModifiers m = evNoModifier) {
        tbControlButton = b;
        tbControlModifiers = m;
    }
///  Set mouse BUTTON and KEY modifier to enable rotation around X axis
///@param[in]  b enum vgButtons : associate your / framework (GLFW/SDL/WIN32/etc)
///                 mouse BUTTON ID
///@param[in]  m enum vgModifiers : associate your / framework (GLFW/SDL/WIN32/etc)
///                 KEY modifier ID : CTRL / ALT / SUPER / SHIFT
///@code
/// static vg::vGizmo3D track;
/// ...
///  track.setGizmoRotXControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) GLFW_MOD_SHIFT);
///  track.setGizmoRotYControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) GLFW_MOD_CONTROL);
///  track.setGizmoRotZControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) GLFW_MOD_SUPER);
///@endcode
    void setGizmoRotXControl( vgButtons b, vgModifiers m = evNoModifier) {
        tbRotationButton = b;
        xRotationModifier = m;
    }
///  Set mouse BUTTON and KEY modifier to enable rotation around Y axis
///@param[in]  b enum vgButtons : associate your / framework (GLFW/SDL/WIN32/etc)
///                 mouse BUTTON ID
///@param[in]  m enum vgModifiers : associate your / framework (GLFW/SDL/WIN32/etc)
///                 KEY modifier ID : CTRL / ALT / SUPER / SHIFT
///@code
/// static vg::vGizmo3D track;
/// ...
///  track.setGizmoRotXControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) GLFW_MOD_SHIFT);
///  track.setGizmoRotYControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) GLFW_MOD_CONTROL);
///  track.setGizmoRotZControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) GLFW_MOD_SUPER);
///@endcode
    //////////////////////////////////////////////////////////////////
    void setGizmoRotYControl( vgButtons b, vgModifiers m = evNoModifier) {
        tbRotationButton = b;
        yRotationModifier = m;
    }
///  Set mouse BUTTON and KEY modifier to enable rotation around Z axis
///@param[in]  b enum vgButtons : associate your / framework (GLFW/SDL/WIN32/etc)
///                 mouse BUTTON ID
///@param[in]  m enum vgModifiers : associate your / framework (GLFW/SDL/WIN32/etc)
///                 KEY modifier ID : CTRL / ALT / SUPER / SHIFT
///@code
/// static vg::vGizmo3D track;
/// ...
///  track.setGizmoRotXControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) GLFW_MOD_SHIFT);
///  track.setGizmoRotYControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) GLFW_MOD_CONTROL);
///  track.setGizmoRotZControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) GLFW_MOD_SUPER);
///@endcode
    void setGizmoRotZControl( vgButtons b, vgModifiers m = evNoModifier) {
        tbRotationButton = b;
        zRotationModifier = m;
    }

/// Returns the quaternion containing current vGizmo3D rotation
///@retval quat : quaternion contain actual rotation
    virtual tQuat getRotation() { return qtRot; }

/// Returns the reference to quaternion containing current vGizmo3D rotation
/// @retval quat& : reference to vGizmo3D quaternion containing actual rotation
/// to acquire and modify, very useful to use directly in ImGuUIZMO_quat
    virtual tQuat &getRotationRef() { return qtRot; }

/// Returns the quaternion containing current vGizmo3D secondary
/// rotation (usually used to rotate light)
/// @retval quat : quaternion contain actual rotation */
    virtual tQuat getSecondRot() { return qtSecondaryRot; }

/// Returns the reference to quaternion containing current vGizmo3D secondary
/// rotation (usually used to rotate light)
/// @retval quat& : reference to vGizmo3D quaternion containing actual rotation
/// to acquire and modify, very useful to use directly in ImGuUIZMO_quat  */
    virtual tQuat &getSecondRotRef() { return qtSecondaryRot; }


    //  get the rotation increment
    //////////////////////////////////////////////////////////////////
    tQuat getStepRotation() { return qtStep; }

/// Set current rotation of vGizmo3D
///@param[in] q quat& : reference quaternion containing rotation to set
    void setRotation(const tQuat &q) { qtRot = q; }

/// Set current rotation of vGizmo3D
///@param[in] q quat& : reference quaternion containing rotation to set
    void setSecondRot(const tQuat &q) { qtSecondaryRot = q; }

    //  get the rotation increment
    //////////////////////////////////////////////////////////////////
    void setStepRotation(const tQuat &q) { qtStep = q; }

    // attenuation<1.0 / increment>1.0 of rotation speed in idle
    //--------------------------------------------------------------------------
    void setIdleRotSpeed(T f) { qIdleSpeedRatio = f;    }
    T    getIdleRotSpeed()    { return qIdleSpeedRatio; }

    //  return current transformations as 4x4 matrix.
    //--------------------------------------------------------------------------
    virtual tMat4 getTransform() = 0;
    //--------------------------------------------------------------------------
    virtual void applyTransform(tMat4 &model) = 0;

// Immediate mode helpers
//--------------------------------------------------------------------

    // for imGuIZMO or immediate mode control
    //////////////////////////////////////////////////////////////////
    void motionImmediateLeftButton( T x, T y, T dx, T dy) {
        tbActive = true;
        delta = tVec2(dx,-dy);
        pos   = tVec2( x,  y);
        update();
    }
    //  for imGuIZMO or immediate mode control
    //////////////////////////////////////////////////////////////////
    virtual void motionImmediateMode( T x, T y, T dx, T dy,  vgModifiers mod) {
        tbActive = true;
        delta = tVec2(dx,-dy);
        pos   = tVec2( x,  y);
        if      (xRotationModifier & mod) { rotationVector = tVec3(T(1), T(0), T(0)); }
        else if (yRotationModifier & mod) { rotationVector = tVec3(T(0), T(1), T(0)); }
        else if (zRotationModifier & mod) { rotationVector = tVec3(T(0), T(0), T(1)); }
        update();
    }
protected:

    tVec2 pos, delta;

    // UI commands that this virtualGizmo responds to (defaults to left mouse button with no modifier key)
    vgButtons   tbControlButton, tbRotationButton;   
    vgModifiers tbControlModifiers, xRotationModifier, yRotationModifier, zRotationModifier;

    //tVec3 rotationVector = tVec3(T(1));

    tQuat qtRot          = tQuat(T(1), T(0), T(0), T(0));
    tQuat qtSecondaryRot = tQuat(T(1), T(0), T(0), T(0));
    tQuat qtStep         = tQuat(T(1), T(0), T(0), T(0));
    tQuat qtIdle         = tQuat(T(1), T(0), T(0), T(0));

#ifdef BACKEND_IS_VULKAN

#else // OPEGL / WEBGL
    tVec3 rotVecModifier = tVec3(1.0);
#endif

    tVec3 rotationVector = tVec3(T(1));
    tVec3 rotationCenter = tVec3(T(0));

    //  settings for the sensitivity
    //////////////////////////////////////////////////////////////////
    T tbScale = T(1);    //base scale sensibility
    T fpsRatio = T(1);   //auto adjust by FPS (call idle with current FPS)
    T qIdleSpeedRatio = T(.33); //autoRotation factor to speedup/slowdown
    
    T minVal;
    tVec3 offset;

    bool tbActive;  // trackbal activated via mouse

    T width, height;
};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//
// virtualGizmoClass
//  trackball: simple mouse rotation control 
//
//--------------------------------------------------------------------
//--------------------------------------------------------------------
TEMPLATE_TYPENAME_T class virtualGizmoClass : public VGIZMO_BASE_CLASS {

public:

    virtualGizmoClass()  { }

    //////////////////////////////////////////////////////////////////
    void motion( T x, T y) { if(this->tbActive) VGIZMO_BASE_CLASS::motion(x,y); }

    //////////////////////////////////////////////////////////////////
    void update() { this->updateGizmo(); }

    //////////////////////////////////////////////////////////////////
    void applyTransform(tMat4 &model) {
        model = translate(model, -this->rotationCenter);
        VGIZMO_BASE_CLASS::applyRotation(model);
        model = translate(model, this->rotationCenter);
    }

    //////////////////////////////////////////////////////////////////
    tMat4 getTransform() {
        tMat4 trans, invTrans, rotation;
        rotation = mat4_cast(this->qtRot);

        trans = translate(tMat4(T(1)),this->rotationCenter);
        invTrans = translate(tMat4(T(1)),-this->rotationCenter);
        
        return invTrans * rotation * trans;
    }

    //  Set the speed for the virtualGizmo.
    //////////////////////////////////////////////////////////////////
    //void setGizmoScale( T scale) { scale = scale; }

    // get the rotation quaternion
    tQuat &getRotationRef() { return this->qtRot; }
};

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//
// virtualGizmo3DClass
//  3D trackball: rotation interface with pan and dolly operations
//
//--------------------------------------------------------------------
//--------------------------------------------------------------------
TEMPLATE_TYPENAME_T class virtualGizmo3DClass : public VGIZMO_BASE_CLASS {

using VGIZMO_BASE_CLASS::delta;
using VGIZMO_BASE_CLASS::qtRot; 

public:
    //////////////////////////////////////////////////////////////////
    virtualGizmo3DClass() : dollyControlButton(evRightButton),   panControlButton(evMiddleButton),  dollyActive(false),
                         dollyControlModifiers(evNoModifier), panControlModifiers(evNoModifier), panActive(false) { }

/// Start/End mouse capture: call on mouse BUTTON event or on state change
///@param[in]  b enum vgButtons : button pressed/released (BUTTON ID)
///@param[in]  m enum vgModifiers : current KEY modifier ID (if active) or evNoModifier = 0
///@param[in]  pressed bool : mouse button pressed => true, released => false
///@param[in]  x T : current X screen coord of mouse cursor
///@param[in]  y T : current Y screen coord of mouse cursor
///@code
///    vg::vGizmo3D track;
///
///     // call on mouse BUTTON event or check BUTTON state change in main render loop
///    track.mouse((vgButtons) button, (vgModifiers) modifier, pressed, x, y);
///@endcode
    void mouse( vgButtons button, vgModifiers mod, bool pressed, T x, T y)
    {
        VGIZMO_BASE_CLASS::mouse(button, mod, pressed,  x,  y);
        if ( button == dollyControlButton && pressed && (dollyControlModifiers ? dollyControlModifiers & mod : dollyControlModifiers == mod) ) {
            dollyActive = true;
            this->activateMouse(x,y);
        }
        else if ( button == dollyControlButton && !pressed) {
            this->deactivateMouse();
            dollyActive = false;
        }
        
        if ( button == panControlButton && pressed && (panControlModifiers ? panControlModifiers & mod : panControlModifiers == mod) ) {
            panActive = true;
            this->activateMouse(x,y);
        }
        else if ( button == panControlButton && !pressed) {
            this->deactivateMouse();
            panActive = false;
        }
    }

    //    Call on wheel (only for Dolly/Zoom)
    //--------------------------------------------------------------------------
    void wheel( T x, T y, T z=T(0)) {
        povPanDollyFactor = z;
        vecPanDolly.z += y * dollyScale * wheelScale * (povPanDollyFactor>T(0) ? povPanDollyFactor : T(1));
    }

    //////////////////////////////////////////////////////////////////
    //void motion( int x, int y, T z=T(0)) { motion( T(x), T(y), z); }
    void motion( T x, T y, T z=T(0)) {
        povPanDollyFactor = z;
        if( this->tbActive || dollyActive || panActive) VGIZMO_BASE_CLASS::motion(x,y);
    }

    //////////////////////////////////////////////////////////////////
    void updatePan() {
        const T pdFactor = (povPanDollyFactor>T(0) ? povPanDollyFactor : T(1));
        vecPanDolly.x += delta.x * panScale * pdFactor;
        vecPanDolly.y += delta.y * panScale * pdFactor;
    }

    //////////////////////////////////////////////////////////////////
    void updateDolly() {
        vecPanDolly.z -= delta.y * dollyScale * (povPanDollyFactor>T(0) ? povPanDollyFactor : T(1));
    }

    //////////////////////////////////////////////////////////////////
    void update() {
        if (this->tbActive) VGIZMO_BASE_CLASS::updateGizmo();
        if (dollyActive) updateDolly();
        if (panActive)   updatePan();
    }

    //////////////////////////////////////////////////////////////////
    void applyTransform(tMat4 &m) {
        m = translate(m, vecPanDolly);
        m = translate(m, -this->rotationCenter);
        VGIZMO_BASE_CLASS::applyRotation(m);
        m = translate(m, this->rotationCenter);
    }

    //////////////////////////////////////////////////////////////////
    tMat4 getTransform() {
        tMat4 trans, invTrans, rotation;
        tMat4 panDollyMat;

        //create pan and dolly translations
        panDollyMat   = translate(tMat4(T(1)),vecPanDolly);

        //create the virtualGizmo rotation
        rotation = mat4_cast(qtRot);

        //create the translations to move the center of rotation to the origin and back
        trans    = translate(tMat4(T(1)), this->rotationCenter);
        invTrans = translate(tMat4(T(1)),-this->rotationCenter);

        //concatenate all the tranforms
        return panDollyMat * invTrans * rotation * trans;
    }
///  Set mouse BUTTON and KEY modifier to control Dolly movements
///@param[in]  b enum vgButtons : associate your / framework (GLFW/SDL/WIN32/etc)
///                 mouse BUTTON ID
///@param[in]  m enum vgModifiers : associate your / framework (GLFW/SDL/WIN32/etc)
///                 KEY modifier ID : CTRL / ALT / SUPER / SHIFT
///@code
/// static vg::vGizmo3D track;
/// ...
///   // Pan and Dolly/Zoom: mouse button and key modifier
///   t.setDollyControl((vgButtons) GLFW_MOUSE_BUTTON_RIGHT, (vgModifiers) 0 /* evNoModifier */);
///   t.setPanControl((vgButtons) GLFW_MOUSE_BUTTON_RIGHT, (vgModifiers) GLFW_MOD_ALT | GLFW_MOD_SHIFT);
///@endcode
    void setDollyControl( vgButtons b, vgModifiers m = evNoModifier) {
        dollyControlButton = b;
        dollyControlModifiers = m;
    }
///  Set mouse BUTTON and KEY modifier to control Pan movements
///@param[in]  b enum vgButtons : associate your / framework (GLFW/SDL/WIN32/etc)
///                 mouse BUTTON ID
///@param[in]  m enum vgModifiers : associate your / framework (GLFW/SDL/WIN32/etc)
///                 KEY modifier ID : CTRL / ALT / SUPER / SHIFT
///@code
/// static vg::vGizmo3D track;
/// ...
///   // Pan and Dolly/Zoom: mouse button and key modifier
///   t.setDollyControl((vgButtons) GLFW_MOUSE_BUTTON_RIGHT, (vgModifiers) 0 /* evNoModifier */);
///   t.setPanControl((vgButtons) GLFW_MOUSE_BUTTON_RIGHT, (vgModifiers) GLFW_MOD_ALT | GLFW_MOD_SHIFT);
///@endcode
    void setPanControl( vgButtons b, vgModifiers m = evNoModifier) {
        panControlButton = b;
        panControlModifiers = m;
    }
    int getPanControlButton() { return panControlButton; }
    int getPanControlModifier() { return panControlModifiers; }



    /// Set mouse wheel sensitivity (in %) for Dolly movements
    /// @param[in]  T scale : sensitivity ==> less < 100 < more
    /// @deprecated will removed on next version: use <b>setPanScale(T scale)</b>
    [[deprecated("Use setWheelScale(T scale) instead.")]]
    void setNormalizedWheelScale( T scale) { wheelScale = scale*constWheelScale;  }

    /// Get current mouse wheel sensitivity (in %) for Dolly movements
    /// @retval  T scale : sensitivity ==> less < 100 < more
    /// @deprecated will removed on next version: use <b>getPanScale()</b>
    [[deprecated("Use getWheelScale() instead.")]]
    T getNormalizedWheelScale() { return wheelScale/constWheelScale;  }

    /// Set mouse sensitivity (in %) for Dolly movements
    /// @param[in]  T scale : sensitivity ==> less < 100 < more
    /// @deprecated will removed on next version: use <b>setPanScale(T scale)</b>
    [[deprecated("Use setDollyScale(T scale) instead.")]]
    void setNormalizedDollyScale(T scale) { dollyScale = scale*constDollyScale;  }

    /// Get current mouse sensitivity (in %) for Dolly movements
    /// @retval  T scale : sensitivity ==> less < 100 < more
    /// @deprecated will removed on next version: use <b>getPanScale()>/b>
    [[deprecated("Use getDollyScale() instead.")]]
    T getNormalizedDollyScale() { return dollyScale/constDollyScale;  }


    /// Set mouse sensitivity (in %) for Pan movements
    /// @param[in]  T scale : sensitivity ==> less < 100 < more
    /// @deprecated will removed on next version: use <b>setPanScale(T scale)</b>
    [[deprecated("Use setPanScale(T scale) instead.")]]
    void setNormalizedPanScale(T scale) { panScale = scale*constPanScale; }

    /// Get current mouse sensitivity (in %) for Pan movements
    /// @retval  T scale : sensitivity ==> less < 100 < more
    /// @deprecated will removed on next version: use <b>getPanScale()</b>
    [[deprecated("Use getPanScale() instead.")]]
    T getNormalizedPanScale() { return panScale/constPanScale; }

    /// Set mouse wheel sensitivity for Dolly movements
    /// @param[in]  T scale : sensitivity ==> less < 1.0 < more
    void setWheelScale( T scale)           { wheelScale = scale;  }

    /// Get current mouse sensitivity for Dolly movements
    /// @retval  T scale : sensitivity ==> less < 1.0 < more
    T getWheelScale()           { return wheelScale;  }

    /// Set mouse sensitivity for Dolly movements
    /// @param[in]  T scale : sensitivity ==> less < 1.0 < more
    void setDollyScale( T scale)           { dollyScale = scale;  }

    /// Get current mouse sensitivity for Dolly movements
    /// @retval  T scale : sensitivity ==> less < 1.0 < more
    T getDollyScale()           { return dollyScale;  }

    /// Set mouse sensitivity for Pan movements
    /// @param[in]  T scale : sensitivity ==> less < 1.0 < more
    void setPanScale( T scale)           { panScale = scale; }

    /// Get current mouse sensitivity for Pan movements
    /// @retval  T scale : sensitivity ==> less < 1.0 < more
    T getPanScale()           { return panScale; }


    //  Set the Dolly to a specified distance.
    //////////////////////////////////////////////////////////////////
    void setDollyPosition(T pos)            { vecPanDolly.z = pos;   }
    void setDollyPosition(const tVec3 &pos) { vecPanDolly.z = pos.z; }

    //  Set the Dolly to a specified distance.
    //////////////////////////////////////////////////////////////////
    void setPanPosition(const tVec3 &pos) { vecPanDolly.x = pos.x; vecPanDolly.y = pos.y;}

    //  Get dolly pos... use as Zoom factor
    //////////////////////////////////////////////////////////////////
    tVec3 getDollyPosition() const { return tVec3 {0, 0, vecPanDolly.z}; }

    //  Get Pan pos... use as Zoom factor
    //////////////////////////////////////////////////////////////////
    tVec3 getPanPosition() const { return tVec3 {vecPanDolly.x, vecPanDolly.y, 0}; }

    //  Get Pan (xy) & Dolly (z) position
    //////////////////////////////////////////////////////////////////
    tVec3 getPosition() { return vecPanDolly; }
    tVec3 &getPositionRef() { return vecPanDolly; }
    void  setPosition(const tVec3 &pos) { vecPanDolly = pos; }

    bool isDollyActive() { return dollyActive; }
    bool isPanActive() { return panActive; }

    void motionImmediateMode( T x, T y, T dx, T dy,  vgModifiers mod) {
        this->tbActive = true;
        this->delta = tVec2(dx,-dy);
        this->pos   = tVec2( x,  y);
        if (dollyControlModifiers & mod) dollyActive = true;
        else if (panControlModifiers & mod) panActive = true;
        update();
    }

private:
    // UI commands that this virtualGizmo responds to (defaults to left mouse button with no modifier key)
    vgButtons   dollyControlButton,    panControlButton;
    vgModifiers dollyControlModifiers, panControlModifiers;

    // Variable used to determine if the manipulator is presently tracking the mouse
    bool dollyActive;
    bool panActive;      

    tVec3 vecPanDolly = tVec3(T(0));

    const T constDollyScale = T(.01);
    const T constPanScale   = T(.01);  //pan scale
    const T constWheelScale = T(7.5);  //dolly multiply for wheel step

    T dollyScale = constDollyScale;  //dolly scale
    T panScale   = constPanScale  ;  //pan scale
    T wheelScale = constWheelScale;  //dolly multiply for wheel step

    T povPanDollyFactor = T(0); // internal use, maintain memory of current distance (pan/zoom speed by distance)
};

#ifdef VGM_USES_TEMPLATE
    #ifdef VGM_USES_DOUBLE_PRECISION
        using vGizmo   = virtualGizmoClass<double>;
        using vGizmo3D = virtualGizmo3DClass<double>;
    #else
        using vGizmo   = virtualGizmoClass<float>;
        using vGizmo3D = virtualGizmo3DClass<float>;
    #endif
    #ifndef IMGUIZMO_USE_ONLY_ROT
        using vImGuIZMO = virtualGizmo3DClass<float>;
    #else
        using vImGuIZMO = virtualGizmoClass<float>;
    #endif
#else
    #ifndef IMGUIZMO_USE_ONLY_ROT
        using vImGuIZMO = virtualGizmo3DClass;
    #else
        using vImGuIZMO = virtualGizmoClass;
    #endif
    using vGizmo    = virtualGizmoClass;
    using vGizmo3D  = virtualGizmo3DClass;
#endif
} // end namespace vg::

#undef T  // if used T as #define, undef it
#undef VGIZMO_H_FILE