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
int width = 1280, height = 800;

#include "dbgValidationLayer.h"
#include "../commons/shadersAndModel.h"


/////////////////////////////////////////////////////////////////////////////
// imGuIZMO: include imGuIZMOquat.h or imguizmo_quat.h
#include <imGuIZMOquat.h> // now also imguizmo_quat.h

mat4 mvpMatrix, viewMatrix, projMatrix;
mat4 cubeObj;


mat4 clipMatrix = mat4(1.0f,  0.0f, 0.0f, 0.0f,
                       0.0f, -1.0f, 0.0f, 0.0f,
                       0.0f,  0.0f, 0.5f, 0.0f,
                       0.0f,  0.0f, 0.5f, 1.0f );  // vulkan clip space has inverted y and half z !
void setPerspective()
{
    float aspectRatio = float(height) / float(width);       // Set "camera" position and perspective
    float fov = radians( 45.0f ) * aspectRatio;
    projMatrix = perspective( fov, 1/aspectRatio, 0.1f, 100.0f );
}

void setScene()
{
    viewMatrix = lookAt( vec3(  0.0f,  0.0f, 10.0f ),   // From / EyePos
                         vec3(  0.0f,  0.0f,  0.0f ),   // To   /
                         vec3(  0.0f,  1.0f,   .0f));   // Up

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

    vk::su::BufferData uniformBufferData( physicalDevice, device, sizeof( mat4x4 ), vk::BufferUsageFlagBits::eUniformBuffer );

        //mat4x4        mvpcMatrix = vk::su::createModelViewProjectionClipMatrix( surfaceData.extent );
    // Expand createModelViewProjectionClipMatrix function...
        float aspectRatio = float(surfaceData.extent.height) / float(surfaceData.extent.width);
        float fov = radians( 45.0f ) * aspectRatio;

        //vk::su::copyToDevice( device, uniformBufferData.deviceMemory, mvpcMatrix ); // move this inside render loop to update vertex buffer every frame

    vk::DescriptorSetLayout descriptorSetLayout =
      vk::su::createDescriptorSetLayout( device, { { vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex } } );
    vk::PipelineLayout pipelineLayout = device.createPipelineLayout( vk::PipelineLayoutCreateInfo( vk::PipelineLayoutCreateFlags(), descriptorSetLayout ) );

    vk::RenderPass renderPass = vk::su::createRenderPass(
      device, vk::su::pickSurfaceFormat( physicalDevice.getSurfaceFormatsKHR( surfaceData.surface ) ).format, depthBufferData.format );

    glslang::InitializeProcess();
    vk::ShaderModule vertexShaderModule   = vk::su::createShaderModule( device, vk::ShaderStageFlagBits::eVertex, vk_vertex_code );
    vk::ShaderModule fragmentShaderModule = vk::su::createShaderModule( device, vk::ShaderStageFlagBits::eFragment, fragment_code );
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
                                                                    renderPass );

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

    // Just get a simpler reference
        GLFWwindow *glfwWindow = surfaceData.window.handle;
    // Initialize ImGui
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

    // Rendering/Draw main loop
        while(!glfwWindowShouldClose(surfaceData.window.handle)) {
            glfwPollEvents();

            glfwGetFramebufferSize(glfwWindow, &width, &height);

    // ImGUI: prepare ImGUI new frame
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

    // ImGui: Your windows here

    // Imgui window (container)
            bool isVisible = true;                             // visibility flag: always on
            ImGui::Begin("##giz", &isVisible);

    // imGuIZMO: declare global/static/member/..
    ///////////////////////////////////
            static quat rotation(1,0,0,0);         // vgMath quat: default constructor initialize @ w(1) x(0) y(0) z(0) ==> w is left/first value
                                                   // for GLM compatibility (if you want switch in future) is necessary an explicit initialization
    // ImGuIZMO.quat widget
    ///////////////////////////////////
            ImGui::gizmo3D("##aaa", rotation,/* size */ 240 ); // if(ImGui::gizmo3D(...) == true) ---> widget has been updated
                                                                   // it returns "only" a rotation (net of transformations) in base to mouse(x,y) movement
                                                                   // and add new rotation, obtained from new "delta.xy" mouse motion, to previous one (saved in your global/static/member var)
    // End Imgui window (container) block
            ImGui::End();

    // now we can transfer the rotation in a matrix... with alternative modes
            mat4 modelMatrix(rotation);                             // constructor
            modelMatrix = mat4_cast(rotation);                      // existing matrix assignation
            modelMatrix = mat4(rotation);                           //    "        "       "
    // build MVP matrix to pass to shader
            mvpMatrix = clipMatrix * projMatrix * viewMatrix * static_cast<mat4>(rotation);   // or use a cast: watch vgMath for all overload operators: quat ==> mat4 / mat3
    // update uniformBuffer: 4x4 matrix ==> vertex shader
            vk::su::copyToDevice( device, uniformBufferData.deviceMemory, mvpMatrix );

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
            clearValues[1].depthStencil = vk::ClearDepthStencilValue( 1.0f, 0 );
            vk::RenderPassBeginInfo renderPassBeginInfo(
              renderPass, framebuffers[currentBuffer.value], vk::Rect2D( vk::Offset2D( 0, 0 ), surfaceData.extent ), clearValues );
            commandBuffer.beginRenderPass( renderPassBeginInfo, vk::SubpassContents::eInline );
            commandBuffer.bindPipeline( vk::PipelineBindPoint::eGraphics, graphicsPipeline );
            commandBuffer.bindDescriptorSets( vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, descriptorSet, nullptr );

            commandBuffer.bindVertexBuffers( 0, vertexBufferData.buffer, { 0 } );
            commandBuffer.setViewport(
              0, vk::Viewport( 0.0f, 0.0f, static_cast<float>( surfaceData.extent.width ), static_cast<float>( surfaceData.extent.height ), 0.0f, 1.0f ) );
            commandBuffer.setScissor( 0, vk::Rect2D( vk::Offset2D( 0, 0 ), surfaceData.extent ) );

            commandBuffer.draw( 12 * 3, 1, 0, 0 );

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
