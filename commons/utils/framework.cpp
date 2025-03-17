//------------------------------------------------------------------------------
//  Copyright (c) 2025 Michele Morrone
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
#include "framework.h"

void frameworkBase::getVulkanFramebufferSize(int *w, int *h) const {
    vk::SurfaceCapabilitiesKHR surfCap;
    if(physicalDevice.getSurfaceCapabilitiesKHR(surface, &surfCap) != vk::Result::eSuccess) exitFailure("Failed acquing surfae size!");
    *w = surfCap.currentExtent.width; *h = surfCap.currentExtent.height;
}

vk::Extent2D frameworkBase::getVulkanFramebufferSize() const {
    vk::SurfaceCapabilitiesKHR surfCap;
    if(physicalDevice.getSurfaceCapabilitiesKHR(surface, &surfCap) != vk::Result::eSuccess) exitFailure("Failed acquing surfae size!");
    return surfCap.currentExtent;
}

void frameworkBase::getSurface(const vk::Instance &instance, const vk::PhysicalDevice &physicalDevice, vk::SurfaceKHR *surface) {
    if (!createSurface(instance, surface))  { exitFailure("Unable to create Vulkan compatible surface"); }
    frameworkBase::surface = *surface;
    frameworkBase::instance = instance;
    frameworkBase::physicalDevice = physicalDevice;
}

void frameworkBase::exitFailure(const char *s) const  {
    if(s) throw std::runtime_error(s);
    throw std::runtime_error("Framework error");
}

/// vGizmo3D initialize:
/// set/associate mouse BUTTON IDs and KEY modifier IDs to vGizmo3D functionalities <br><br>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void frameworkBase::initVGizmo3D(vg::vGizmo3D &vgTrackball)     // Settings to control vGizmo3D
{
    // Initialization is necessary to associate your preferences to vGizmo3D
    // These are also the DEFAULT values, so if you want to maintain these combinations you can omit they
    // and to override only the associations that you want modify
        vgTrackball.setGizmoRotControl      (vg::evButton1  /* or vg::evLeftButton */, 0 /* vg::evNoModifier */ );
    // Rotations around specific axis: mouse button and key modifier
        vgTrackball.setGizmoRotXControl     (vg::evButton1  /* or vg::evLeftButton */, vg::evShiftModifier);
        vgTrackball.setGizmoRotYControl     (vg::evButton1  /* or vg::evLeftButton */, vg::evControlModifier);
        vgTrackball.setGizmoRotZControl     (vg::evButton1  /* or vg::evLeftButton */, vg::evAltModifier | vg::evSuperModifier);
    // Set vGizmo3D control for secondary rotation
        vgTrackball.setGizmoSecondRotControl(vg::evButton2  /* or vg::evRightButton */, 0 /* vg::evNoModifier */ );
    // Pan and Dolly/Zoom: mouse button and key modifier
        vgTrackball.setDollyControl         (vg::evButton2 /* or vg::evRightButton */, vg::evControlModifier);
        vgTrackball.setPanControl           (vg::evButton2 /* or vg::evRightButton */, vg::evShiftModifier);
    // N.B. vg::enums are ONLY mnemonic: select and pass specific vg::enum to framework (that can have also different IDs)

    // passing the screen sizes auto-set the mouse sensitivity
        int w, h;
        getFramebufferSize(&w, &h);        
        vgTrackball.viewportSize(w, h);         // is necessary also to call when resize window/surface: re-calibrate drag rotation & auto-set mouse sensitivity
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

#if defined(APP_USES_SDL2) || defined(APP_USES_SDL3)
frameworkSDL::frameworkSDL(int32_t width, int32_t height, const char *title, uint32_t flags, const char *fmk) :
    frameworkBase(title, fmk) {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow(sTitle.c_str(),
#ifdef APP_USES_SDL2
        SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,
#endif
        width, height, flags);
    if(window == nullptr) { exitFailure("SDL: create Window"); }

    getReqExtensions();
}

void frameworkSDL::getReqExtensions() {
    uint32_t extensionCount {0};
#ifdef APP_USES_SDL2
    if (!SDL_Vulkan_GetInstanceExtensions(getWindow(), &extensionCount, nullptr)) exitFailure("GetInstanceExtensions: getCount...");
    extensions.resize(extensionCount) ;
    if (!SDL_Vulkan_GetInstanceExtensions(getWindow(), &extensionCount, extensions.data())) exitFailure("GetInstanceExtensions: getData...");
#else  // SDL3
    const char* const* sdl_extensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
    for (uint32_t n = 0; n < extensionCount; n++)
        extensions.push_back(sdl_extensions[n]);
#endif
    addReqExtensions();
}

bool frameworkSDL::pollEvents() {
    //SDL_PumpEvents();
    while(SDL_PollEvent(&event)) {
        imguiProcessEvent(event);
        if(event.type == APP_SDL_QUIT || checkWindowCloseEvent())
            { return false; }
        if(checkWindowResizeEvent())
            { }
    }
    return true;
}

void frameworkSDL::addReqExtensions() {
#ifdef ENABLE_VALIDATION_LAYER
    extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
}

/// vGizmo3D: Check key modifier currently pressed (GLFW version)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int frameworkSDL::getVGizmo3DKeyModifier() {
    SDL_Keymod keyMod = SDL_GetModState();
#ifdef APP_USES_SDL2
    if(     keyMod & KMOD_CTRL )     return vg::evControlModifier;
    else if(keyMod & KMOD_SHIFT)     return vg::evShiftModifier;
    else if(keyMod & KMOD_ALT  )     return vg::evAltModifier;
    else if(keyMod & KMOD_GUI  )     return vg::evSuperModifier;
#else  // APP_USES_SDL3
    if(     keyMod & SDL_KMOD_CTRL ) return vg::evControlModifier;
    else if(keyMod & SDL_KMOD_SHIFT) return vg::evShiftModifier;
    else if(keyMod & SDL_KMOD_ALT  ) return vg::evAltModifier;
    else if(keyMod & SDL_KMOD_GUI  ) return vg::evSuperModifier;
#endif
    else return vg::evNoModifier;
}

// vGizmo3D: check changing button state to activate/deactivate drag movements  (pressing both activate/deacivate both functionality)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void frameworkSDL::checkVGizmo3DMouseEvent(vg::vGizmo3D &vgTrackball) {
    static int leftPress = 0, rightPress = 0, middlePress = 0;
    if(!ImGui::GetIO().WantCaptureMouse) {
#ifdef APP_USES_SDL2
        int x, y;
#else  // APP_USES_SDL3
        float x, y;
#endif
        int mouseState = SDL_GetMouseState(&x, &y);
        if(leftPress != (mouseState & SDL_BUTTON_LMASK)) {                  // check if leftButton state is changed
            leftPress =  mouseState & SDL_BUTTON_LMASK ;                    // set new (different!) state
            vgTrackball.mouse(vg::evLeftButton, getVGizmo3DKeyModifier(),   // send communication to vGizmo3D...
                                          leftPress, x, y);                 // ... checking if a key modifier currently is pressed
        }
        if(rightPress != (mouseState & SDL_BUTTON_RMASK)) {                 // check if rightButton state is changed
            rightPress =  mouseState & SDL_BUTTON_RMASK;                    // set new (different!) state
            vgTrackball.mouse(vg::evRightButton, getVGizmo3DKeyModifier(),  // send communication to vGizmo3D...
                                           rightPress, x, y);               // ... checking if a key modifier currently is pressed
        }
        // Simulating a double press (left+right button) using MIDDLE button,
        // sending two "consecutive" activation/deactivation to rotate cube and light spot together
        if(middlePress != (mouseState & SDL_BUTTON_MMASK)) {             // check if middleButton state is changed
            middlePress =  mouseState & SDL_BUTTON_MMASK;                // set new (different!) middle button state
            vgTrackball.mouse(vg::evRightButton, getVGizmo3DKeyModifier(), middlePress, x, y);  // call Right activation/deactivation with same "middleStatus"
            vgTrackball.mouse(vg::evLeftButton,  getVGizmo3DKeyModifier(), middlePress, x, y);  // call Left  activation/deactivation with same "middleStatus"
        }
// vGizmo3D: if "drag" active update internal rotations (primary and secondary)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        vgTrackball.motion(x,y);
    }
}

#else
GLFWmonitor* getCurrentMonitor(GLFWwindow *window)
{
    int nmonitors, bestoverlap {0};;
    GLFWmonitor *bestmonitor = nullptr;
    GLFWmonitor **monitors = glfwGetMonitors(&nmonitors);

    int wx, wy; glfwGetWindowPos (window, &wx, &wy);
    int ww, wh; glfwGetWindowSize(window, &ww, &wh);

    for(int i = 0; i < nmonitors; i++) {
        const GLFWvidmode *mode = glfwGetVideoMode(monitors[i]);
        int mx, my; glfwGetMonitorPos(monitors[i], &mx, &my);

        const int overlap = std::max(0, std::min(wx + ww, mx + mode->width ) - std::max(wx, mx)) *
                            std::max(0, std::min(wy + wh, my + mode->height) - std::max(wy, my));

        if(bestoverlap < overlap) {
            bestoverlap = overlap;
            bestmonitor = monitors[i];
        }
    }
    return bestmonitor ? bestmonitor : throw "no monitor found!";
}

void toggleFullscreenOnOff(GLFWwindow* window)
{
    static int windowed_xpos, windowed_ypos, windowed_width, windowed_height;

    if (glfwGetWindowMonitor(window))
        glfwSetWindowMonitor(window, nullptr, windowed_xpos , windowed_ypos,
                                              windowed_width, windowed_height, 0);
    else
        if(GLFWmonitor* monitor = getCurrentMonitor(window)) {
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            glfwGetWindowPos(window, &windowed_xpos, &windowed_ypos);
            glfwGetWindowSize(window, &windowed_width, &windowed_height);
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        }
}

frameworkClass::frameworkClass(void *ptrApp, int32_t width, int32_t height, const char *title) :
    frameworkBase(title, ":GLFW") {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(width, height, sTitle.c_str(), nullptr, nullptr);
    if(window == nullptr) { exit(EXIT_FAILURE); }

    glfwSetWindowUserPointer(getWindow(), ptrApp);

    centerWindow();

    if (!glfwVulkanSupported()) { exitFailure("GLFW: Vulkan Not Supported"); }

    getReqExtensions();
}

void frameworkClass::centerWindow() {
    int w, h; glfwGetFramebufferSize(getWindow(),&w, &h);
    const GLFWvidmode *mode = glfwGetVideoMode(getCurrentMonitor(getWindow()));
    glfwSetWindowPos(getWindow(), (mode->width-w)>>1, (mode->height-h)>>1);
}

void frameworkClass::getReqExtensions() {
    uint32_t extensionCount = 0;
    const char** ptrExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);
    extensions = std::vector(ptrExtensions, ptrExtensions + extensionCount);
#ifdef ENABLE_VALIDATION_LAYER
    extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
}

/// vGizmo3D: Check key modifier currently pressed (GLFW version)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int frameworkClass::getVGizmo3DKeyModifier()
{
    if((glfwGetKey(getWindow(),GLFW_KEY_LEFT_CONTROL)    == GLFW_PRESS) || (glfwGetKey(getWindow(),GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS))
            return vg::evControlModifier;
    else if((glfwGetKey(getWindow(),GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) || (glfwGetKey(getWindow(),GLFW_KEY_RIGHT_SHIFT)   == GLFW_PRESS))
            return vg::evShiftModifier;
    else if((glfwGetKey(getWindow(),GLFW_KEY_LEFT_ALT)   == GLFW_PRESS) || (glfwGetKey(getWindow(),GLFW_KEY_RIGHT_ALT)     == GLFW_PRESS))
            return vg::evAltModifier;
    else if((glfwGetKey(getWindow(),GLFW_KEY_LEFT_SUPER) == GLFW_PRESS) || (glfwGetKey(getWindow(),GLFW_KEY_RIGHT_SUPER)   == GLFW_PRESS))
            return vg::evSuperModifier;
    else return vg::evNoModifier;
}

/// vGizmo3D: check changing button state to activate/deactivate drag movements (pressing together left/right activate/deactivate both)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void frameworkClass::checkVGizmo3DMouseEvent(vg::vGizmo3D &vgTrackball)
{
        static int leftPress = 0, rightPress = 0, middlePress = 0;
    if(!ImGui::GetIO().WantCaptureMouse) {
        double x, y;
        glfwGetCursorPos(getWindow(), &x, &y);
        if(glfwGetMouseButton(getWindow(), GLFW_MOUSE_BUTTON_LEFT) != leftPress) {  // check if leftButton state is changed
            leftPress = leftPress == GLFW_PRESS ? GLFW_RELEASE : GLFW_PRESS;        // set new (different!) state
            vgTrackball.mouse(vg::evLeftButton, getVGizmo3DKeyModifier(),           // send communication to vGizmo3D...
                                          leftPress, x, y);                         // ... checking if a key modifier currently is pressed
        }
        if(glfwGetMouseButton(getWindow(), GLFW_MOUSE_BUTTON_RIGHT) != rightPress) { // same thing for rightButton
            rightPress = rightPress == GLFW_PRESS ? GLFW_RELEASE : GLFW_PRESS;
            vgTrackball.mouse(vg::evRightButton, getVGizmo3DKeyModifier(),
                                           rightPress, x, y);
        }
        // Just a trik: simulating a double press (left+right button together) using MIDDLE button,
        // sending two "consecutive" activation/deactivation calls to rotate cube and light spot together
        if(glfwGetMouseButton(getWindow(), GLFW_MOUSE_BUTTON_MIDDLE) != middlePress) {         // check if middleButton state is changed
            middlePress = middlePress == GLFW_PRESS ? GLFW_RELEASE : GLFW_PRESS;               // set new (different!) middle button state
            vgTrackball.mouse(vg::evLeftButton, getVGizmo3DKeyModifier(),  middlePress, x, y); // call Left activation/deactivation with same "middleStatus"
            vgTrackball.mouse(vg::evRightButton, getVGizmo3DKeyModifier(), middlePress, x, y); // call Right activation/deactivation with same "middleStatus"
        }

    // vGizmo3D: if "drag" active update internal rotations (primary and secondary)
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        vgTrackball.motion(x,y);
        //vgTrackball.motion(x,y,vgTrackball.getDollyPosition().z);
    }
}
#endif