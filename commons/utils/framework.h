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
#pragma once
#include <vulkan/vulkan.hpp>
#include <shaderc/shaderc.hpp>

#include <limits>

// declare before "imgui" includes, or anywhere if IMGUI_DEFINE_MATH_OPERATORS
#include <imguizmo_quat.h>

#ifdef APP_USES_SDL2
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_vulkan.h>
    #include "imgui/backends/imgui_impl_sdl2.h"
    #define APP_SDL_QUIT SDL_QUIT
#else
    #ifdef APP_USES_SDL3
        #include <SDL3/SDL.h>
        #include <SDL3/SDL_vulkan.h>
        #include "imgui/backends/imgui_impl_sdl3.h"
        #define APP_SDL_QUIT SDL_EVENT_QUIT
    #else
        #include "imgui/backends/imgui_impl_glfw.h"
        #include <GLFW/glfw3.h>
    #endif
#endif


class frameworkBase
{
public:
    frameworkBase(const char *title, const char *fmk) : sTitle(title) { sTitle+=fmk; }
    virtual ~frameworkBase() = default;
    void exitFailure(const char *s = nullptr) const;

    std::vector<const char*> &getExtensions() { return extensions; }

    virtual void destroyWindow() const = 0;
    virtual void quit()          const = 0;
    virtual bool createSurface(const vk::Instance &instance, vk::SurfaceKHR *surface) = 0;
    virtual bool pollEvents() = 0;
    virtual void getFramebufferSize(int *w, int *h) const = 0;
    virtual int  getVGizmo3DKeyModifier() = 0;
    virtual void checkVGizmo3DMouseEvent(vg::vGizmo3D &vgTrackball) = 0;


    void getVulkanFramebufferSize(int *w, int *h) const;
    vk::Extent2D getVulkanFramebufferSize() const;

    void getSurface(const vk::Instance &instance, const vk::PhysicalDevice &physicalDevice, vk::SurfaceKHR *surface);

/// vGizmo3D initialize:
/// set/associate mouse BUTTON IDs and KEY modifier IDs to vGizmo3D functionalities <br><br>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void initVGizmo3D(vg::vGizmo3D &track);

protected:
    vk::Instance instance;
    vk::PhysicalDevice physicalDevice;
    vk::SurfaceKHR surface;
    void* window = nullptr;
    std::vector<const char*> extensions;
    std::string sTitle;
    bool isReqResize = false;
};

#if defined(APP_USES_SDL2) || defined(APP_USES_SDL3)
class frameworkSDL : public frameworkBase {
public:
    frameworkSDL(int32_t width, int32_t height, const char *title, uint32_t flags, const char *fmk);

    bool windowShouldClose() const { return shouldQuit; }
    // Mouse events
    bool isMouseLeftButtonDown()  const { return isMouseButtonDown(SDL_BUTTON_LEFT) ; }
    bool isMouseRightButtonDown() const { return isMouseButtonDown(SDL_BUTTON_RIGHT); }

    //void getFramebufferSize(int *w, int *h) const { SDL_GetWindowSize(getWindow(), w, h); }
    //void getFramebufferSize(int *w, int *h) const { SDL_Vulkan_GetDrawableSize(getWindow(), w, h); }
    void getFramebufferSize(int *w, int *h) const { getVulkanFramebufferSize(w, h); }

    void getReqExtensions();

    bool pollEvents();

    void destroyWindow() const { SDL_DestroyWindow(getWindow());  }
    void quit()          const { SDL_Quit(); }

    virtual bool checkWindowResizeEvent()                           = 0;
    virtual bool checkWindowCloseEvent()                            = 0;
    virtual bool isMouseButtonDown(uint32_t b) const                = 0;

    virtual void imguiNewFrame()                                    = 0;
    virtual void imguiShutdown()                                    = 0;
    virtual void imguiProcessEvent(SDL_Event &event)                = 0;
    virtual void imguiInitForVulkan(bool install_callbacks = true)  = 0;


/// vGizmo3D: Check key modifier currently pressed (GLFW version)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    int getVGizmo3DKeyModifier();
    void checkVGizmo3DMouseEvent(vg::vGizmo3D &vgTrackball);


    [[nodiscard]] SDL_Window* getWindow() const { return (SDL_Window*) window; }

#define PTR_NAME_SDL "sdlApp"
    //static void setUserPointer(void *wnd, void *ptr) { SDL_SetWindowData(static_cast<SDL_Window*>(wnd), PTR_NAME_SDL, ptr); }
    //static void *getUserPointer(void *wnd) { return SDL_GetWindowData(static_cast<SDL_Window*>(wnd), PTR_NAME_SDL); }
#undef PTR_NAME_SDL

protected:
    void addReqExtensions();

    bool shouldQuit = false;
    SDL_Event event;
};

#if defined(APP_USES_SDL2)
class frameworkClass : public frameworkSDL {
public:
    frameworkClass(void *ptrApp, int32_t width, int32_t height, const char *title) :
        frameworkSDL(width, height, title, SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE, ":SDL2") { }

    bool checkWindowCloseEvent()  { return event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(getWindow()); }
    bool checkWindowResizeEvent() { return event.type == SDL_WINDOWEVENT_RESIZED || event.type == SDL_WINDOWEVENT_SIZE_CHANGED; }

    void getMousePos(int *x, int *y) const { SDL_GetMouseState(x, y); }
    bool isMouseButtonDown(uint32_t b) const { int x,y; return SDL_GetMouseState(&x, &y) == SDL_BUTTON(b); }

    bool createSurface(const vk::Instance &instance, vk::SurfaceKHR *surface) { return SDL_Vulkan_CreateSurface(getWindow(), instance, (VkSurfaceKHR *) surface); }

    void imguiNewFrame() { ImGui_ImplSDL2_NewFrame(); }
    void imguiShutdown() { ImGui_ImplSDL2_Shutdown(); }
    void imguiProcessEvent(SDL_Event &event) { ImGui_ImplSDL2_ProcessEvent(&event); }
    void imguiInitForVulkan(bool install_callbacks = true) { ImGui_ImplSDL2_InitForVulkan(getWindow()); }


};
#endif
#if defined(APP_USES_SDL3)
class frameworkClass : public frameworkSDL {
public:
    frameworkClass(void *ptrApp, int32_t width, int32_t height, const char *title) :
        frameworkSDL(width, height, title, SDL_WINDOW_VULKAN |  SDL_WINDOW_RESIZABLE, ":SDL3") { }

    bool checkWindowCloseEvent()  { return event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(getWindow()); }
    bool checkWindowResizeEvent() { return event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED || event.type == SDL_EVENT_WINDOW_RESIZED; }

    bool isMouseButtonDown(uint32_t b) const { float x,y; return SDL_GetMouseState(&x, &y) == SDL_BUTTON_MASK(b); }
    void getMousePos(int *retX, int *retY) const { float x, y; SDL_GetMouseState(&x, &y); *retX = x+.5; *retY = y+.5; }

    bool createSurface(const vk::Instance &instance, vk::SurfaceKHR *surface) { return SDL_Vulkan_CreateSurface(getWindow(), instance, nullptr, (VkSurfaceKHR *) surface); }

    void imguiNewFrame() { ImGui_ImplSDL3_NewFrame(); }
    void imguiShutdown() { ImGui_ImplSDL3_Shutdown(); }
    void imguiProcessEvent(SDL_Event &event) { ImGui_ImplSDL3_ProcessEvent(&event); }
    void imguiInitForVulkan(bool install_callbacks = true) { ImGui_ImplSDL3_InitForVulkan(getWindow()); }

};
#endif
#else
// GLFW
/////////////////////////////////////////////
class frameworkClass : public frameworkBase {
public:
    frameworkClass(void *ptrApp, int32_t width, int32_t height, const char *title);
    ~frameworkClass() override {  }

    void centerWindow();

    bool isMouseLeftButtonDown()  const { return glfwGetMouseButton(getWindow(), GLFW_MOUSE_BUTTON_LEFT)  == GLFW_PRESS; }
    bool isMouseRightButtonDown() const { return glfwGetMouseButton(getWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS; }

    void getFramebufferSize(int *w, int *h) const { glfwGetFramebufferSize(getWindow(),w, h); }
    void getMousePos(int *retX, int *retY) {  double x, y;  glfwGetCursorPos(getWindow(), &x, &y); *retX = x+.5; *retY = y+.5; }

    bool windowShouldClose() const { return glfwWindowShouldClose(getWindow()); }
    bool pollEvents() { glfwPollEvents(); return !glfwWindowShouldClose(getWindow()); }

    void destroyWindow() const { glfwDestroyWindow(getWindow());  }
    void quit() const { glfwTerminate(); }

    static void setUserPointer(void *wnd, void *ptr) { glfwSetWindowUserPointer(static_cast<GLFWwindow*>(wnd), ptr); }
    static void *getUserPointer(void *wnd) { return glfwGetWindowUserPointer(static_cast<GLFWwindow*>(wnd)); }
    bool createSurface(const vk::Instance &instance, vk::SurfaceKHR *surface) {
        return glfwCreateWindowSurface(instance, getWindow(), nullptr, (VkSurfaceKHR *) surface) == VK_SUCCESS;  }

    [[nodiscard]] GLFWwindow* getWindow() const { return static_cast<GLFWwindow*>(window); }


    void imguiNewFrame() const { ImGui_ImplGlfw_NewFrame(); }
    void imguiShutdown() const { ImGui_ImplGlfw_Shutdown(); }
    void imguiInitForVulkan(bool install_callbacks = true) const { ImGui_ImplGlfw_InitForVulkan(getWindow(), install_callbacks); }



/// vGizmo3D: Check key modifier currently pressed (GLFW version)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    int getVGizmo3DKeyModifier();
    void checkVGizmo3DMouseEvent(vg::vGizmo3D &vgTrackball);

    // callbacks
    /////////////////////////////////////////////
    static void resizeCallback(GLFWwindow* window, int w, int h) {
        //const auto app = static_cast<vkApp*>(getUserPointer());
        //app->resizeWnd();
        //std::cerr << "Window resized" << std::endl;
        //app->wndResized = true;
    }

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
private:
    void getReqExtensions();
};

GLFWmonitor* getCurrentMonitor(GLFWwindow *window);
void toggleFullscreenOnOff(GLFWwindow* window);
/*
class frameworkApp final : public frameworkClass
{
public:
    frameworkApp(void *ptrApp, const int wndDimX = 512, const int wndDimY = 512, const char *title = nullptr) :
        frameworkClass(wndDimX, wndDimY, title ? title : "App") { glfwSetWindowUserPointer(getWindow(), ptrApp); }

};*/
#endif

class frameworkApp final : public frameworkClass
{
public:
    frameworkApp(void *ptrApp, const int wndDimX = 512, const int wndDimY = 512, const char *title = nullptr) :
        frameworkClass(ptrApp, wndDimX, wndDimY, title ? title : "App") {  }

};

