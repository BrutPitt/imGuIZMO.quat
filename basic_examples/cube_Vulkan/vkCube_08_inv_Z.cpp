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
//
// Modified from original vulkanHpp sample:
// https://github.com/KhronosGroup/Vulkan-Hpp/tree/main/samples/15_DrawCube
//
// Copyright(c) 2019, NVIDIA CORPORATION. All rights reserved.
// Licensed under the Apache License, Version 2.0

#include "utils/math.hpp"
#include "utils/shaders.hpp"
#include "utils/utils.hpp"
#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/Public/ShaderLang.h>

#include <iostream>
#include <thread>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_vulkan.h>
#include <imgui/imgui_internal.h>
#include <GLFW/glfw3.h>
#include "imgui/imgui_impl_glfw.h"

static char const * AppName    = "vkCube";
static char const * EngineName = "ImGuIZMO";

#include "dbgValidationLayer.h"
#include "../commons/shadersAndModel.h"


/////////////////////////////////////////////////////////////////////////////
// imGuIZMO: include imGuIZMOquat.h or imguizmo_quat.h
#include <imGuIZMOquat.h> // now also imguizmo_quat.h

struct _uboMat {
    mat4 mvpMatrix;
    mat4 lightMatrix;
} uboMat;

int width = 1280, height = 800;
GLFWwindow *glfwWindow;

const int nVertex = sizeof(coloredCubeData)/(sizeof(float)*2);
int nElemVtx = 4;

mat4 mvpMatrix, viewMatrix, projMatrix;
mat4 lightObj, lightMatrix, cubeObj;

vec3 lightPos(2, 1.5, 2.5);        // Light Position

mat4 clipMatrix = mat4(1.0f,  0.0f, 0.0f, 0.0f,
                       0.0f, -1.0f, 0.0f, 0.0f,
                       0.0f,  0.0f,-1.0f, 0.0f,    // invert Z
                       0.0f,  0.0f, 1.0f, 1.0f );  // vulkan clip space inverted y and z


// +Z set depthBufferCompareOp = vk::CompareOp::eLessOrEqual    and depthBufferClearValue = 1.0
// -Z set depthBufferCompareOp = vk::CompareOp::eGreaterOrEqual and depthBufferClearValue = 0.0
vk::CompareOp depthBufferCompareOp = vk::CompareOp::eLessOrEqual;  // of vk::PipelineDepthStencilStateCreateInfo
float depthBufferClearValue = 1.0;                                 // "depth" value of vk::ClearDepthStencilValue of vk::RenderPassBeginInfo

// ... or ... (not both!)

// +Z set vert_T = vk_vertex_instanced, frag_T = fragment_code
// -Z set vert_T = vk_vert_inv_z      , frag_T = vk_frag_inv_z
const char *vert_T = vk_vert_inv_z;
const char *frag_T = vk_frag_inv_z;


/// imGuIZMO / vGizmo3D : declare global/static/member/..
///////////////////////////////////
vg::vGizmo3D track;     // using vGizmo3D global/static/member instead of specifics variables...
                        // have rotations & Pan/Dolly position variables inside to use with imGuIZMO.quat
                        // And it's necessary if you want use also direct-screen manipulator

mat4 compensateView; // compensate rotation of viewMatrix lookAt Matrix


/// vGizmo3D initialize: set/associate button IDs and KEY modifier IDs to vGizmo3D functionalities
///////////////////////////////////
void initVGizmo3D()     // Settings to control vGizmo3D
{
    // Initialization are necessary to associate specific (GLFW/SDL/etc) frameworks DEFINES/values to control
        track.setGizmoRotControl( vg::evLeftButton  /* or vg::evButton1 */, 0 /* vg::evNoModifier */ );
    // Rotations around specific axis: mouse button and key modifier
        track.setGizmoRotXControl(vg::evLeftButton  /* or vg::evButton1 */, vg::evShiftModifier);
        track.setGizmoRotYControl(vg::evLeftButton  /* or vg::evButton1 */, vg::evControlModifier);
        track.setGizmoRotZControl(vg::evLeftButton  /* or vg::evButton1 */, vg::evAltModifier | vg::evSuperModifier);
    // Pan and Dolly/Zoom: mouse button and key modifier
        track.setDollyControl(    vg::evRightButton /* or vg::evButton2 */, 0 /* vg::evNoModifier */);
        track.setPanControl(      vg::evRightButton /* or vg::evButton2 */, vg::evControlModifier | vg::evShiftModifier);
    // passing the screen sizes auto-set the mouse sensitivity
        track.viewportSize(width, height);      // but if you need to more feeling with the mouse use:
    // track.setGizmoFeeling(1.0);              // 1.0 default,  > 1.0 more sensible, < 1.0 less sensible

    // setIdleRotSpeed(1.0)                     // If used Idle() feature (continue rotation on Idle) it set that speed: more speed > 1.0 ,  less < 1.0

    // other settings if you need it
    /////////////////////////////////////////////
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
//*****************************************
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

void setPerspective()
{
    float aspectRatio = float(height) / float(width);       // Set "camera" position and perspective
    float fov = radians( 45.0f ) * aspectRatio;
    projMatrix = perspective( fov, 1/aspectRatio, 0.1f, 100.0f );
}

void setScene()
{
    viewMatrix = lookAt( vec3( 10.0f, 10.0f, 10.0f ),   // From / EyePos
                         vec3(  0.0f,  0.0f,  0.0f ),   // To   /
                         vec3(  3.0f,  1.0f,   .0f));   // Up

    // Now scale cube to better view light position
    cubeObj = mat4(1); // nothing to do ... scale(mat4(1), vec3(.5));

/// imGuIZMO / vGizmo3D
///////////////////////////////////
    //track.setRotation(quat(1,0,0,0));                     // vGizmo3D with NO initial rotation (default initialization)
    //track.setRotation(eulerAngleXYZ(vec3(radians(45),
    //                                     radians( 0),
    //                                     radians( 0))));  // vGizmo3D with rotation of 45 degrees on X axis
    track.setRotation(viewMatrix);                          // vGizmo3D with ViewMatrix (lookAt) rotation

    // for Pan & Dolly always bounded on screen coords (x = left/right, y = up/douw, z = in/out) we remove viewMatrix rotation
    // otherwise Pan & Dolly have as reference the Cartesian axes
    compensateView = inverse(mat4_cast(quat(viewMatrix)));


    // light model

    // acquiring rotation for the light pos
    const float len = length(lightPos);
     //if(len<1.0 && len>= FLT_EPSILON) { normalize(lightPos); len = 1.0; }  // controls are not necessary: lightPos is known
     //else if(len > FLT_EPSILON)
        quat q = angleAxis(acosf(-lightPos.x/len), normalize(vec3(FLT_EPSILON, lightPos.z, -lightPos.y)));
    track.setSecondRot(q);          // store secondary rotation for the Light

    lightObj = translate(mat4(1), lightPos);
    lightObj = scale(lightObj, vec3(.1));       // using same cube vertex but with 10% size

    lightObj = inverse(static_cast<mat4>(track.getSecondRot())) * lightObj ;

    setPerspective();
}

std::vector<const char*> extensions;

void getReqExtensions() {
    uint32_t extensionCount = 0;
    const char** ptrExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);
    extensions = std::vector(ptrExtensions, ptrExtensions + extensionCount);
#ifndef NDEBUG
    extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
}

int main( int /*argc*/, char ** /*argv*/ )
{
  try
  {
    // original example select surface extensions @ CMake time (no via GLFW/SDL): UNIX (XCB_KHR), WIN32 (WIN32_KHR), APPLE (METAL_EXT)
    // so I have forced framework Initialization @ startup (and just some little changes in NVIDIA "utils.*" files")
    vk::su::WindowData window = vk::su::createWindow( AppName, vk::Extent2D( width, height ) );

    getReqExtensions();

    vk::Instance instance = vk::su::createInstance( AppName, EngineName, {}, extensions );
    debugMessengersLayers debug; // debug messenger obj

    // and use my Validation Layer Code
    CHECK_VALIDATION_LAYER_SUPPORT()
    BUILD_DEBUG_MESSENGER(instance)

    vk::PhysicalDevice physicalDevice = instance.enumeratePhysicalDevices().front();
    // just modified window size
    vk::su::SurfaceData surfaceData( instance, AppName, vk::Extent2D( width, height ), window );

    std::pair<uint32_t, uint32_t> graphicsAndPresentQueueFamilyIndex = vk::su::findGraphicsAndPresentQueueFamilyIndex( physicalDevice, surfaceData.surface );
    vk::Device                    device = vk::su::createDevice( physicalDevice, graphicsAndPresentQueueFamilyIndex.first, vk::su::getDeviceExtensions() );

    vk::CommandPool   commandPool = device.createCommandPool( { vk::CommandPoolCreateFlagBits::eResetCommandBuffer , graphicsAndPresentQueueFamilyIndex.first } );
    vk::CommandBuffer commandBuffer =
      device.allocateCommandBuffers( vk::CommandBufferAllocateInfo( commandPool, vk::CommandBufferLevel::ePrimary, 1 ) ).front();

    vk::Queue graphicsQueue = device.getQueue( graphicsAndPresentQueueFamilyIndex.first, 0 );
    vk::Queue presentQueue  = device.getQueue( graphicsAndPresentQueueFamilyIndex.second, 0 );

    vk::su::SwapChainData swapChainData( physicalDevice,
                                         device,
                                         surfaceData.surface,
                                         surfaceData.extent,
                                         vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc,
                                         {},
                                         graphicsAndPresentQueueFamilyIndex.first,
                                         graphicsAndPresentQueueFamilyIndex.second );

    vk::su::DepthBufferData depthBufferData( physicalDevice, device, vk::Format::eD32Sfloat, surfaceData.extent );

    vk::su::BufferData uniformBufferData( physicalDevice, device, sizeof( _uboMat ), vk::BufferUsageFlagBits::eUniformBuffer );

        //vk::su::copyToDevice( device, uniformBufferData.deviceMemory, mvpcMatrix ); // move this statement inside render loop to update vertex buffer every frame

    vk::DescriptorSetLayout descriptorSetLayout =
      vk::su::createDescriptorSetLayout( device, { { vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex } } );
    vk::PipelineLayout pipelineLayout = device.createPipelineLayout( vk::PipelineLayoutCreateInfo( vk::PipelineLayoutCreateFlags(), descriptorSetLayout ) );

    vk::RenderPass renderPass = vk::su::createRenderPass(
      device, vk::su::pickSurfaceFormat( physicalDevice.getSurfaceFormatsKHR( surfaceData.surface ) ).format, depthBufferData.format );

    glslang::InitializeProcess();
    vk::ShaderModule vertexShaderModule   =  vk::su::createShaderModule( device, vk::ShaderStageFlagBits::eVertex, vert_T );
    vk::ShaderModule fragmentShaderModule =  vk::su::createShaderModule( device, vk::ShaderStageFlagBits::eFragment, frag_T );
    glslang::FinalizeProcess();

    std::vector<vk::Framebuffer> framebuffers =
      vk::su::createFramebuffers( device, renderPass, swapChainData.imageViews, depthBufferData.imageView, surfaceData.extent );

    vk::su::BufferData vertexBufferData( physicalDevice, device, sizeof( coloredCubeData ), vk::BufferUsageFlagBits::eVertexBuffer );
    vk::su::copyToDevice( device, vertexBufferData.deviceMemory, coloredCubeData, sizeof( coloredCubeData ) / sizeof( coloredCubeData[0] ) );

    vk::DescriptorPool            descriptorPool = vk::su::createDescriptorPool( device, { { vk::DescriptorType::eUniformBuffer, 1 } } );
    vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo( descriptorPool, descriptorSetLayout );
    vk::DescriptorSet             descriptorSet = device.allocateDescriptorSets( descriptorSetAllocateInfo ).front();

    vk::su::updateDescriptorSets( device, descriptorSet, { { vk::DescriptorType::eUniformBuffer, uniformBufferData.buffer, VK_WHOLE_SIZE, {} } }, {} );

    vk::PipelineCache pipelineCache    = device.createPipelineCache( vk::PipelineCacheCreateInfo() );
    vk::Pipeline      graphicsPipeline = vk::su::createGraphicsPipeline( device,
                                                                    pipelineCache,
                                                                    std::make_pair( vertexShaderModule, nullptr ),
                                                                    std::make_pair( fragmentShaderModule, nullptr ),
                                                                    sizeof( coloredCubeData[0] ),
                                                                    { { vk::Format::eR32G32B32A32Sfloat, 0 }, { vk::Format::eR32G32B32A32Sfloat, 16 } },
                                                                    vk::FrontFace::eClockwise,
                                                                    true,
                                                                    pipelineLayout,
                                                                    renderPass,
                                                                    depthBufferCompareOp);

    // Moved Out of the loop
    vk::Semaphore             imageAcquiredSemaphore = device.createSemaphore( vk::SemaphoreCreateInfo() );
    vk::Fence drawFence = device.createFence( vk::FenceCreateInfo() );

    // "RE-GET" minImgCount (used locally/internally from original example)
    vk::SurfaceCapabilitiesKHR surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR( surfaceData.surface );
    uint32_t minImageCount = vk::su::clampSurfaceImageCount( 3u, surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount );

    // v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Start ImGui & ImGuIZMO.quad code

    // scene initialization
        setScene();

    // vGizmo3D: initialize
    ///////////////////////////////////
        initVGizmo3D();

    // Just get a simpler reference
        GLFWwindow *glfwWindow = surfaceData.window.handle;

    //***********************************************
    // ImGui start VULKAN-HPP initialization
        const std::array pool_sizes { vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, 1) } ; // array declaration C++ 17
        vk::DescriptorPoolCreateInfo pool_info(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, //flags
                                               1 /* .maxSets */, pool_sizes.size(), pool_sizes.data() /*, .pNext = nullptr  */ );
        vk::DescriptorPool imguiPool;
        vk::detail::resultCheck(device.createDescriptorPool( &pool_info, nullptr, &imguiPool), "ImGui: createDescripptorPool...");

    // Setup/Initialize Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        ImGui::StyleColorsDark(); // ImGui style: or ImGui::StyleColorsLight();

        ImGuiStyle& style = ImGui::GetStyle();
        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForVulkan(glfwWindow, true);  // callback ImGui active: you don't even need to handle mouse events
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = instance;
        init_info.PhysicalDevice = physicalDevice;
        init_info.Device = device;
        init_info.QueueFamily = graphicsAndPresentQueueFamilyIndex.first;
        init_info.Queue = graphicsQueue;
        init_info.PipelineCache = pipelineCache;
        init_info.DescriptorPool = imguiPool;
        init_info.RenderPass = renderPass;                   // Ignored if using dynamic rendering
        init_info.UseDynamicRendering = false;
        init_info.Subpass = 0;
        init_info.MinImageCount = minImageCount;
        init_info.ImageCount = swapChainData.images.size();
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.Allocator = nullptr;
        init_info.CheckVkResultFn = nullptr;
        ImGui_ImplVulkan_Init(&init_info);

        ImGui_ImplVulkan_SetMinImageCount(minImageCount);
    // ImGui end VULKAN-HPP initialization
    //***********************************************

    // imGuIZMO: set mouse feeling and mods
    ///////////////////////////////////
        imguiGizmo::setGizmoFeelingRot(2.f);                    // default 1.0, >1 more mouse sensitivity, <1 less mouse sensitivity
        imguiGizmo::setPanScale(3.5f);                          // default 1.0, >1 more, <1 less
        imguiGizmo::setDollyScale(3.5f);                        // default 1.0, >1 more, <1 less
        imguiGizmo::setDollyWheelScale(7.0f);                   // default 2.0, > more, < less ... (from v3.1 separate values)
        imguiGizmo::setPanModifier(vg::evControlModifier);      // change KEY modifier: CTRL (default)
        imguiGizmo::setDollyModifier(vg::evShiftModifier);     // change KEY modifier: SHIFT (default)


    // Rendering/Draw main loop
        while(!glfwWindowShouldClose(surfaceData.window.handle)) {
            glfwPollEvents();

            glfwGetFramebufferSize(glfwWindow, &width, &height);

    // vGizmo3D: is necessary intercept mouse event not destined to ImGui
    ///////////////////////////////////
        if(!ImGui::GetIO().WantCaptureMouse) {
            static int leftPress = 0, rightPress = 0;
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
            if(leftPress == GLFW_PRESS || rightPress == GLFW_PRESS)
                track.motion(x,y);                                                       // if one button is pressed vGizmo3D catch the motion
        }
    // vGizmo3D: call it every rendering loop if you want a continue rotation until you do not click on screen
    ///////////////////////////////////
        track.idle();   // set continuous rotation on Idle: the smooth rotation depends on speed of last mouse movements
                        // It can be adjusted from setIdleRotSpeed(1.0) > more speed, < less
                        // It can be stopped by click on screen (without mouse movement)

    // ImGUI: prepare ImGUI new frame
            ImGui_ImplVulkan_NewFrame();
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
                                          ImGuiWindowFlags_NoScrollbar);


    // colored text for display quat(w,x,y,z) components
        ImGui::SetCursorPos(ImVec2(0,0));
        ImGui::PushItemWidth(widgetSize*.25-2);
        ImGui::TextColored(ImVec4(1,1,1,1), "w: % 1.2f", track.getRotation().w); ImGui::SameLine();
        ImGui::TextColored(ImVec4(1,0,0,1), "x: % 1.2f", track.getRotation().x); ImGui::SameLine();
        ImGui::TextColored(ImVec4(0,1,0,1), "y: % 1.2f", track.getRotation().y); ImGui::SameLine();
        ImGui::TextColored(ImVec4(0,0,1,1), "z: % 1.2f", track.getRotation().z);
        ImGui::PopItemWidth();

    // ImGuIZMO.quat widget
    ///////////////////////////////////
        ImGui::gizmo3D("##aaa", track.getRotationRef(), track.getSecondRotRef(), widgetSize); // if(ImGui::gizmo3D(...) == true) ---> widget has been updated

    // ImGuIZMO.quat with also pan and Dolly/zoom
    ///////////////////////////////////
        ImGui::gizmo3D("##a01", track.getPositionRef(), track.getRotationRef(), widgetSize);    // Ctrl+LButton = Pan ... Shift+LButton = Dolly/Zoom

    // End Imgui window (container) block
        ImGui::End();
        style.WindowBorderSize = prevWindowBorderSize;              // restore border size
        ImGui::PopStyleColor();                                     // frame color (pushed)
        ImGui::PopStyleColor();                                     // Background (pushed)

    // Build a "translation" matrix
        mat4 translationMatrix = translate(mat4(1), track.getPosition());      // add translations (pan/dolly) to an identity matrix

    // build MVP matrix to pass to shader
        uboMat.mvpMatrix   = clipMatrix * projMatrix * viewMatrix * compensateView * translationMatrix * cubeObj * static_cast<mat4>(track.getRotation());
        uboMat.lightMatrix = clipMatrix * projMatrix * viewMatrix * compensateView * translationMatrix * (static_cast<mat4>(track.getSecondRot())) * lightObj;
    // update uniformBuffer: 4x4 matrix ==> vertex shader
            vk::su::copyToDevice( device, uniformBufferData.deviceMemory, uboMat );

    // ImGui: Render/draw
            ImGui::Render();

    /* VULKAN_KEY_START */
            // Get the index of the next available swapchain image:
            vk::ResultValue<uint32_t> currentBuffer = device.acquireNextImageKHR( swapChainData.swapChain, vk::su::FenceTimeout, imageAcquiredSemaphore, nullptr );
            assert( currentBuffer.result == vk::Result::eSuccess );
            assert( currentBuffer.value < framebuffers.size() );

            device.resetCommandPool(commandPool);
            commandBuffer.reset();
            commandBuffer.begin( vk::CommandBufferBeginInfo( vk::CommandBufferUsageFlags() ) );

            std::array<vk::ClearValue, 2> clearValues;
            clearValues[0].color        = vk::ClearColorValue( 0.07f, 0.07f, 0.07f, 0.07f );
            clearValues[1].depthStencil = vk::ClearDepthStencilValue( depthBufferClearValue, 0 );
            vk::RenderPassBeginInfo renderPassBeginInfo(
              renderPass, framebuffers[currentBuffer.value], vk::Rect2D( vk::Offset2D( 0, 0 ), surfaceData.extent ), clearValues );
            commandBuffer.beginRenderPass( renderPassBeginInfo, vk::SubpassContents::eInline );
            commandBuffer.bindPipeline( vk::PipelineBindPoint::eGraphics, graphicsPipeline );
            commandBuffer.bindDescriptorSets( vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, descriptorSet, nullptr );

            commandBuffer.bindVertexBuffers( 0, vertexBufferData.buffer, { 0 } );
            commandBuffer.setViewport(
              0, vk::Viewport( 0.0f, 0.0f, static_cast<float>( surfaceData.extent.width ), static_cast<float>( surfaceData.extent.height ), 0.0f, 1.0f ) );
            commandBuffer.setScissor( 0, vk::Rect2D( vk::Offset2D( 0, 0 ), surfaceData.extent ) );

            commandBuffer.draw( 12 * 3, 2, 0, 0 );      //  

    // ImGui: just before endRenderPass insert ImGui CommandBuffer data
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

            commandBuffer.endRenderPass();
            commandBuffer.end();

            vk::PipelineStageFlags waitDestinationStageMask( vk::PipelineStageFlagBits::eColorAttachmentOutput );
            vk::SubmitInfo         submitInfo( imageAcquiredSemaphore, waitDestinationStageMask, commandBuffer );
            graphicsQueue.submit( submitInfo, drawFence );

            while ( vk::Result::eTimeout == device.waitForFences( drawFence, VK_TRUE, vk::su::FenceTimeout ) ) ;
    // is necessary reset fences before submit it again (now there we are in a loop)
            vk::detail::resultCheck(device.resetFences(1, &drawFence), "resetFences...");

            vk::Result result = presentQueue.presentKHR( vk::PresentInfoKHR( {}, swapChainData.swapChain, currentBuffer.value ) );
            switch ( result )
            {
              case vk::Result::eSuccess: break;
              case vk::Result::eSuboptimalKHR: std::cout << "vk::Queue::presentKHR returned vk::Result::eSuboptimalKHR !\n"; break;
              default: assert( false );  // an unexpected result is returned !
            }
        }

    // Free and destroy ImGui resources
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();                  // SDL: ImGui_ImplSDL2/3_Shutdown() | GLFW: ImGui_ImplGlfw_Shutdown()
        ImGui::DestroyContext();
        device.destroyDescriptorPool( imguiPool );  // destroy ImGui DescriptorPool

    // End ImGui & ImGuIZMO.quad code
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^


    // not more necessary
    //std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );

    device.waitIdle();

    device.destroyFence( drawFence );
    device.destroySemaphore( imageAcquiredSemaphore );

    /* VULKAN_KEY_END */

    device.destroyPipeline( graphicsPipeline );
    device.destroyPipelineCache( pipelineCache );
    device.destroyDescriptorPool( descriptorPool );
    vertexBufferData.clear( device );
    for ( auto framebuffer : framebuffers )
    {
      device.destroyFramebuffer( framebuffer );
    }
    device.destroyShaderModule( fragmentShaderModule );
    device.destroyShaderModule( vertexShaderModule );
    device.destroyRenderPass( renderPass );
    device.destroyPipelineLayout( pipelineLayout );
    device.destroyDescriptorSetLayout( descriptorSetLayout );
    uniformBufferData.clear( device );
    depthBufferData.clear( device );
    swapChainData.clear( device );
    device.destroyCommandPool( commandPool );
    device.destroy();
    instance.destroySurfaceKHR( surfaceData.surface );
    DESTROY_DEBUG_MESSENGER(instance)
    instance.destroy();

    // Cleanup Framework
    glfwDestroyWindow( window.handle );
    glfwTerminate();
  }
  catch ( vk::SystemError & err )
  {
    std::cout << "vk::SystemError: " << err.what() << std::endl;
    exit( -1 );
  }
  catch ( std::exception & err )
  {
    std::cout << "std::exception: " << err.what() << std::endl;
    exit( -1 );
  }
  catch ( ... )
  {
    std::cout << "unknown error\n";
    exit( -1 );
  }
  return 0;
}
