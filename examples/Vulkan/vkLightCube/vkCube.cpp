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
#include <vulkan/vulkan.hpp>
#include <shaderc/shaderc.hpp>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <fstream>
#include <set>
#include <limits>
#include <array>
#include <cfloat>

#include "vkCube.h"

/////////////////////////////////////////////////////////////////////////////
// imGuIZMO: include imGuIZMOquat.h or imguizmo_quat.h
#include <imGuIZMOquat.h> // now also imguizmo_quat.h

#include <imgui/backends/imgui_impl_vulkan.h>


void renderWidgets(vg::vGizmo3D &track, vec3& vLight, int width, int height);


// passed from CMake: SPIR_V_EXT is different for DEBUG | RELEASE

#ifndef APP_SHADERS_DIR
    #define APP_SHADERS_DIR Shaders
#endif

#ifdef NDEBUG
    #define SPIR_V_EXT ".spirv"
#else
    #define SPIR_V_EXT ".dbg.spirv"
#endif

#define FRAG_NAME "vkLightCube.frag" SPIR_V_EXT
#define VERT_NAME "vkLightCube.vert" SPIR_V_EXT

#define _STRING(x) #x
#define STRING(x) _STRING(x)

static std::vector<uint32_t> readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) { throw std::runtime_error("failed to open file"); }

    const auto fileSize = file.tellg();
    std::vector<uint32_t> buffer(fileSize);

    file.seekg(0);
    file.read((char *)buffer.data(), fileSize);
    file.close();

    return buffer;
}

uint32_t getGraphicsIndex(const vk::PhysicalDevice &physicalDevice)
{
    auto qFamProp = physicalDevice.getQueueFamilyProperties();
    return std::distance(qFamProp.begin(), std::find_if(qFamProp.begin(), qFamProp.end(), [](vk::QueueFamilyProperties const& prop) { return prop.queueFlags & vk::QueueFlagBits::eGraphics; }));
}

uint32_t getPresentIndex(const vk::PhysicalDevice &physicalDevice, const vk::SurfaceKHR &surface)
{
    uint32_t qFamPropSize = physicalDevice.getQueueFamilyProperties().size();
    for(uint32_t i = 0; i < qFamPropSize; i++)
        if(physicalDevice.getSurfaceSupportKHR(i, surface)) return i;
    throw std::runtime_error("No present surface supported\n");
}

void vkAppBase::createInstance()
{
    vk::ApplicationInfo appInfo(appName, 1, nullptr, 0, VK_API_VERSION_1_1);
    vk::InstanceCreateInfo instInfo({}, &appInfo, 0, nullptr, uint32_t(framework.getExtensions().size()), framework.getExtensions().data());

    instance = vk::createInstance(instInfo); // create a Instance
#if defined(ENABLE_VALIDATION_LAYER) && defined ADDITIONAL_INFO
    std::cout << "Available extensions:" << std::endl;
    for(const auto& extension : vk::enumerateInstanceExtensionProperties()) std::cout << "\t" << extension.extensionName << std::endl;
#endif
}

void vkAppBase::selectPhysicalDevice()
{
    const auto physicalDevices = instance.enumeratePhysicalDevices();
    if(physicalDevices.empty()) { throw std::runtime_error("no GPUs with Vulkan support!"); }

    PRINT_AVAILABLE_DEVICES(physicalDevices)  // print all devices aviable in debug mode
    physicalDevice = physicalDevices.front(); // Select only first device
}

void vkAppBase::initLogicalDevice()
{
    graphQueueFamilyIdx             = getGraphicsIndex(physicalDevice);
    const uint32_t presentQueueFamilyIdx  = getPresentIndex(physicalDevice, surface);

    std::set uniqueQueueFamilyIndices = { graphQueueFamilyIdx, presentQueueFamilyIdx };
    std::vector<uint32_t> familyIndices = { uniqueQueueFamilyIndices.begin(), uniqueQueueFamilyIndices.end() };

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

    constexpr float queuePriority = 0.0f;
    for(uint32_t queueFamilyIndex : uniqueQueueFamilyIndices)
        queueCreateInfos.push_back(vk::DeviceQueueCreateInfo({}, uint32_t(queueFamilyIndex), 1, &queuePriority));

    const std::vector deviceExtensions { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    const vk::DeviceCreateInfo deviceInfo({}, queueCreateInfos.size(), queueCreateInfos.data(),
                                              debug.validationLayers.size(), nullptr /* deprecated debug.validationLayers.data()*/, // only if Debug enabled -> check class
                                              deviceExtensions.size(), deviceExtensions.data());
    logicalDevice = physicalDevice.createDevice(deviceInfo);

    sm = graphQueueFamilyIdx == presentQueueFamilyIdx ? sharingModeStruct({ vk::SharingMode::eExclusive , 0u, nullptr              }) :
                                                        sharingModeStruct({ vk::SharingMode::eConcurrent, 2u, familyIndices.data() });

    commandPool   = logicalDevice.createCommandPool(vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, uint32_t(graphQueueFamilyIdx)));

    queueGraphics = logicalDevice.getQueue(graphQueueFamilyIdx, 0);
    queuePresent  = logicalDevice.getQueue(presentQueueFamilyIdx, 0);
}

void vkAppBase::builSwapchain(vk::SwapchainKHR oldSwapChain)
{
    swapChainExtent = vk::Extent2D(width, height);
    const vk::SwapchainCreateInfoKHR swapChainInfo({}, surface, minImageCount, surfaceFormat, vk::ColorSpaceKHR::eSrgbNonlinear, swapChainExtent, 1, vk::ImageUsageFlagBits::eColorAttachment,
            sm.sharingMode, sm.idxCount, sm.idxDataPtr, vk::SurfaceTransformFlagBitsKHR::eIdentity, vk::CompositeAlphaFlagBitsKHR::eOpaque, pmType, true, oldSwapChain);

    swapChain       = logicalDevice.createSwapchainKHR(swapChainInfo);
    swapChainImages = logicalDevice.getSwapchainImagesKHR(swapChain);
}

void vkAppBase::buildDepthBuffer()
{
    depthBuffer.init(physicalDevice, logicalDevice, vk::Format::eD32Sfloat, vk::Extent2D(width,height));
}

void vkAppBase::buildImagesView()
{
    scImageView.clear();
    constexpr vk::ComponentMapping componentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA);
    constexpr vk::ImageSubresourceRange imgSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1) ;
    for (const auto image : swapChainImages) {
        const vk::ImageViewCreateInfo imageViewInfo({}, image, vk::ImageViewType::e2D, surfaceFormat, componentMapping, imgSubresourceRange);
        scImageView.push_back(logicalDevice.createImageView(imageViewInfo));
    }
}

void vkAppBase::loadSpirVShaders()
{
    const auto vertCode = readFile(STRING(APP_SHADERS_DIR) "/" VERT_NAME);
    const auto fragCode = readFile(STRING(APP_SHADERS_DIR) "/" FRAG_NAME);

    vertShaderMod = logicalDevice.createShaderModule(vk::ShaderModuleCreateInfo({}, vertCode.size(), vertCode.data()));
    fragShaderMod = logicalDevice.createShaderModule(vk::ShaderModuleCreateInfo({}, fragCode.size(), fragCode.data()));
}

// Load and compile shaders, like in WebGL/OpenGL
/////////////////////////////////////////////
void vkAppBase::compileShaders()
{
    const auto vertCode = readFile(VERT_NAME);
    const auto fragCode = readFile(FRAG_NAME);

    shaderc::Compiler compiler;
    shaderc::CompileOptions options;
#ifdef NDEBUG
    options.SetOptimizationLevel(shaderc_optimization_level_performance);
#else
    options.SetOptimizationLevel(shaderc_optimization_level_zero);
#endif
    shaderc::SpvCompilationResult vertShaderModule = compiler.CompileGlslToSpv((char *)vertCode.data(), shaderc_glsl_vertex_shader, "vertex shader", options);
    if(vertShaderModule.GetCompilationStatus() != shaderc_compilation_status_success) std::cerr << vertShaderModule.GetErrorMessage();

    const auto vertShaderCode = std::vector<uint32_t>{ vertShaderModule.cbegin(), vertShaderModule.cend() };
    const vk::ShaderModuleCreateInfo vertShaderCreateInfo({}, vertShaderCode.size()*sizeof(uint32_t), vertShaderCode.data());
    vertShaderMod = logicalDevice.createShaderModule(vertShaderCreateInfo);

    const shaderc::SpvCompilationResult fragShaderModule = compiler.CompileGlslToSpv((char *)fragCode.data(), shaderc_glsl_fragment_shader, "fragment shader", options);
    if (fragShaderModule.GetCompilationStatus() != shaderc_compilation_status_success) std::cerr << fragShaderModule.GetErrorMessage();

    const auto fragShaderCode = std::vector<uint32_t>{ fragShaderModule.cbegin(), fragShaderModule.cend() };
    vk::ShaderModuleCreateInfo fragShaderCreateInfo({}, fragShaderCode.size()*sizeof(uint32_t), fragShaderCode.data());
    fragShaderMod = logicalDevice.createShaderModule(fragShaderCreateInfo);
}

void vkAppBase::buildRenderPass()
{
    static std::vector<vk::AttachmentDescription> attachDescriptions;

    attachDescriptions.emplace_back(vk::AttachmentDescriptionFlags(), surfaceFormat,
                                    vk::SampleCountFlagBits::e1,     vk::AttachmentLoadOp::eClear,     vk::AttachmentStoreOp::eStore,
                                    vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined,
                                    vk::ImageLayout::ePresentSrcKHR);

    if(depthBuffer.format != vk::Format::eUndefined)
        attachDescriptions.emplace_back(vk::AttachmentDescriptionFlags(), depthBuffer.format,
                                    vk::SampleCountFlagBits::e1,     vk::AttachmentLoadOp::eClear,     vk::AttachmentStoreOp::eStore,
                                    vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined,
                                    vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::AttachmentReference colorAttachmentRef(0, vk::ImageLayout::eColorAttachmentOptimal);
    vk::AttachmentReference depthAttachment( 1, vk::ImageLayout::eDepthStencilAttachmentOptimal );

    vk::SubpassDescription subpassDescription({}, vk::PipelineBindPoint::eGraphics, {}, colorAttachmentRef, {}, depthBuffer.format != vk::Format::eUndefined  ? &depthAttachment : nullptr);
    renderPass = logicalDevice.createRenderPass(vk::RenderPassCreateInfo({}, attachDescriptions, subpassDescription));
}

void vkAppBase::createPipelineCache()
{
    vk::PipelineCacheCreateInfo pipelineCacheCreateInfo;
    pipelineCache = logicalDevice.createPipelineCache(pipelineCacheCreateInfo);
}

void vkAppBase::buildGraphPipeline()
{
    vk::PipelineInputAssemblyStateCreateInfo assemblyInfo({}, vk::PrimitiveTopology::eTriangleList);

    vk::PipelineRasterizationStateCreateInfo rastering { {}, {}, {}, vk::PolygonMode::eFill, vk::CullModeFlagBits::eBack, vk::FrontFace::eClockwise, {}, {}, {}, {}, 1.0f };

    vk::ColorComponentFlags colorComponentFlags( vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA );
    vk::PipelineColorBlendAttachmentState cbAttachment( false, vk::BlendFactor::eZero, vk::BlendFactor::eZero, vk::BlendOp::eAdd,
                                                               vk::BlendFactor::eZero, vk::BlendFactor::eZero, vk::BlendOp::eAdd, colorComponentFlags );

    vk::PipelineColorBlendStateCreateInfo colorBlending({}, false, vk::LogicOp::eNoOp, cbAttachment, { { 1.0f, 1.0f, 1.0f, 1.0f } } );

    vk::PipelineViewportStateCreateInfo viewportState( {}, 1, {}, 1, {} );

    std::vector dynamicStateFlags { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
    vk::PipelineDynamicStateCreateInfo dynamicState({}, dynamicStateFlags.size(), dynamicStateFlags.data());

    bool depthBuffered = true;
    vk::StencilOpState stencilOpState( vk::StencilOp::eKeep, vk::StencilOp::eKeep, vk::StencilOp::eKeep, vk::CompareOp::eAlways );
    vk::PipelineDepthStencilStateCreateInfo depthStencil({}, depthBuffered, depthBuffered, depthBufferCompareOp, false, false, stencilOpState, stencilOpState );

    vk::PipelineMultisampleStateCreateInfo multisampling { {}, vk::SampleCountFlagBits::e1 };


    vk::PipelineShaderStageCreateInfo vertShaderStageInfo({}, vk::ShaderStageFlagBits::eVertex  , vertShaderMod, "main");
    vk::PipelineShaderStageCreateInfo fragShaderStageInfo({}, vk::ShaderStageFlagBits::eFragment, fragShaderMod, "main");
    std::array pipelineShaderStages = { vertShaderStageInfo, fragShaderStageInfo };

    std::vector<vk::VertexInputAttributeDescription> vertexInputAttributeDescriptions;
    vk::PipelineVertexInputStateCreateInfo           vtxStateInfo;
    vk::VertexInputBindingDescription                vertexInputBindingDescription( 0, vertexStride );

    if ( 0 < vertexStride ) {
        vertexInputAttributeDescriptions.reserve( vertexInputAttributeFormatOffset.size() );
        for ( uint32_t i = 0; i < vertexInputAttributeFormatOffset.size(); i++ )
            vertexInputAttributeDescriptions.emplace_back( i, 0, vertexInputAttributeFormatOffset[i].first, vertexInputAttributeFormatOffset[i].second );
        vtxStateInfo.setVertexBindingDescriptions( vertexInputBindingDescription );
        vtxStateInfo.setVertexAttributeDescriptions( vertexInputAttributeDescriptions );
    }

    vk::GraphicsPipelineCreateInfo pipelineCreateInfo({}, pipelineShaderStages, &vtxStateInfo, &assemblyInfo, nullptr,
        &viewportState, &rastering, &multisampling, &depthStencil, &colorBlending, &dynamicState, pipelineLayout, renderPass );

    pipeline = logicalDevice.createGraphicsPipeline(pipelineCache, pipelineCreateInfo).value;
}

void vkAppBase::createSyncItems()
{
    fence.resize(minImageCount);
    vk::SemaphoreCreateInfo semaphoreCreateInfo;
    for(size_t i = 0; i < minImageCount; i++)
        fence[i] = logicalDevice.createFence({vk::FenceCreateFlagBits::eSignaled});
    semaphoreImageAvailable = logicalDevice.createSemaphore(semaphoreCreateInfo);
    semaphoreEndRendering   = logicalDevice.createSemaphore(semaphoreCreateInfo);
}

void vkAppBase::buildFramebuffer()
{
    vk::ImageView imgviewBuffers[2];
    imgviewBuffers[1] = depthBuffer.imageView;

    bool isDepthPresent = true;

    scFrameBuffers.resize(minImageCount);
    for(size_t i = 0; i < minImageCount; i++) {
        imgviewBuffers[0] = scImageView[i];
        scFrameBuffers[i] = logicalDevice.createFramebuffer(vk::FramebufferCreateInfo({}, renderPass, isDepthPresent ? 2 : 1, imgviewBuffers, width, height, 1));
    }
}

void vkAppBase::createDescriptorsPool()
{
    vk::DescriptorPoolSize poolSize(vk::DescriptorType::eUniformBuffer, 2);
    descriptorPool = logicalDevice.createDescriptorPool(vk::DescriptorPoolCreateInfo({}, 1, poolSize));
}

void vkAppBase::setupDescriptorsSetLayout()
{
    std::vector<vk::DescriptorSetLayoutBinding> layoutBindings {
        { 0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex  },
        { 1, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eFragment} };

    descriptorSetLayout = logicalDevice.createDescriptorSetLayout(vk::DescriptorSetLayoutCreateInfo({}, layoutBindings));
    pipelineLayout      = logicalDevice.createPipelineLayout(     vk::PipelineLayoutCreateInfo({}, descriptorSetLayout));
}

void vkAppBase::updateDescriptorsSets(std::vector<bufferSet> &buffer)
{
    descriptorSets = logicalDevice.allocateDescriptorSets(vk::DescriptorSetAllocateInfo(descriptorPool, descriptorSetLayout));
    std::vector<vk::DescriptorBufferInfo> desc;
    for( auto &b : buffer) desc.emplace_back(b.buffer, 0, b.bufferSize);

    vk::DescriptorSet descriptorSet = descriptorSets.front();
    std::vector<vk::WriteDescriptorSet> writeDescriptorSet {
        {descriptorSet, 0, 0, 1, vk::DescriptorType::eUniformBuffer, {}, &desc[0]},
        {descriptorSet, 1, 0, 1, vk::DescriptorType::eUniformBuffer, {}, &desc[1]} };
    logicalDevice.updateDescriptorSets(writeDescriptorSet, {});
}

void vkAppBase::buildCommandBuffers()
{
    commandBuffer.resize(swapChainImages.size());
    commandBuffer = logicalDevice.allocateCommandBuffers(vk::CommandBufferAllocateInfo(commandPool, vk::CommandBufferLevel::ePrimary, commandBuffer.size()));
}

void vkAppBase::setCommandBuffer(uint32_t currentFrame)
{
    std::array<vk::ClearValue, 2> clearValues;
    clearValues[0].color        = vk::ClearColorValue( 0.07f, 0.07f, 0.07f, 0.07f );
    clearValues[1].depthStencil = vk::ClearDepthStencilValue( depthBufferClearValue, 0 );
    const vk::Rect2D rect(vk::Offset2D( 0, 0 ), swapChainExtent);
    const vk::Rect2D scissor({ 0, 0 }, swapChainExtent);
    const vk::Viewport viewport(0.0f, 0.0f, swapChainExtent.width, swapChainExtent.height, 0.0f, 1.0f);

    logicalDevice.resetCommandPool(commandPool);
    commandBuffer[currentFrame].reset();
    commandBuffer[currentFrame].begin(vk::CommandBufferBeginInfo());
    commandBuffer[currentFrame].beginRenderPass(vk::RenderPassBeginInfo(renderPass, scFrameBuffers[currentBufferIdx], rect, clearValues), vk::SubpassContents::eInline);
    commandBuffer[currentFrame].setViewport(0, 1, &viewport);
    commandBuffer[currentFrame].setScissor(0, 1, &scissor);
    commandBuffer[currentFrame].bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
    commandBuffer[currentFrame].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, 1, &descriptorSets[0], 0, nullptr );
    commandBuffer[currentFrame].bindVertexBuffers(0, vtxCubeData.buffer, {0 } );

    commandBuffer[currentFrame].draw( 12 * 3, 2, 0, 0 );      //

    // ImGui: just before endRenderPass insert ImGui CommandBuffer data
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer[currentFrame]);

    commandBuffer[currentFrame].endRenderPass();
    commandBuffer[currentFrame].end();
}

void vkAppBase::rebuildAllSwapchain()
{
    // wait GPU to idle before destroy resources
    logicalDevice.waitIdle();

    // Rebuil swapchain
    vk::SwapchainKHR oldSwapChain = swapChain;
    builSwapchain(oldSwapChain);
    logicalDevice.destroySwapchainKHR(oldSwapChain);

    // rebuild: depthBuffer, frameBuffer, commandBuffer and syncObj
    destroySwapChainComponents();
    buildSwapChainComponents();
}

void vkApp::setPerspective()
{
    float aspectRatio = float(height) / float(width);       // Set "camera" position and perspective
    float fov = radians( 45.0f ) * aspectRatio;
    // PERSPECTIVE: depends from clipMatrix (+Z or -Z) ==> view in the end of vkCube.h
    projMatrix = perspective( fov, 1/aspectRatio, 0.1f, 100.0f );

}

// light model
vec3 lightPos(2, 2.5, 3);        // Light Position
void vkApp::setScene()
{
    // LOOK_AT: depends from clipMatrix (+Z or -Z) ==> view in the end of vkCube.h
    viewMatrix = LOOK_AT( vec3(12.0f,  6.0f,  4.0f),   // From / EyePos / PoV
                          vec3( 0.0f,  0.0f,  0.0f),   // To   /  Tgt
                          vec3( 3.0f,  1.0f,   .0f) ); // Up

    // Now scale cube to better view light position
    cubeObj = mat4(1); // nothing to do ... scale( vec3(.5));

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// vGizmo3D initialize:
/// set/associate mouse BUTTON IDs and KEY modifier IDs to vGizmo3D functionalities <br><br>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    framework.initVGizmo3D(vgTrackball);

/// imGuIZMO / vGizmo3D
/// Set initial rotation
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //vgTrackball.setRotation(quat(1,0,0,0));                   // vGizmo3D with NO initial rotation (DEFAULT)
    //vgTrackball.setRotation(eulerAngleXYZ(vec3(radians(45),
    //                                     radians( 0),
    //                                     radians( 0))));      // vGizmo3D with rotation of 45 degrees on X axis
    vgTrackball.setRotation(viewMatrix);                        // vGizmo3D with ViewMatrix (lookAt) rotation

    // for Pan & Dolly always bounded on screen coords (x = left/right, y = up/douw, z = in/out) we remove viewMatrix rotation
    // otherwise Pan & Dolly have as reference the Cartesian axes
    compensateView = inverse(mat4_cast(quat(viewMatrix)));


    // acquiring rotation for the light pos
    const float len = length(lightPos);
     //if(len<1.0 && len>= FLT_EPSILON) { normalize(lightPos); len = 1.0; }  // controls are not necessary: lightPos is known
     //else if(len > FLT_EPSILON)
        quat q = angleAxis(acosf(-lightPos.x/len), normalize(vec3(FLT_EPSILON, lightPos.z, -lightPos.y)));
    vgTrackball.setSecondRot(q);          // store secondary rotation for the Light

    lightObj = translate(mat4(1), lightPos);
    lightObj = inverse(static_cast<mat4>(vgTrackball.getSecondRot())) * lightObj ;

    setPerspective();
}

void vkApp::draw()
{
    if(!prepareFrame()) { resizeWnd(); return; } // acquireNextImageKHR ==> resizeWnd on vk::OutOfDateKHRError

    const uint32_t currentFrame = currentBufferIdx; // used to differentiate in example: in reality they do not always coincide
    
    vk::detail::resultCheck(logicalDevice.waitForFences(1, &fence[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max()), "waitForFences...");
    vk::detail::resultCheck(logicalDevice.resetFences  (1, &fence[currentFrame]), "resetFences...");

    setCommandBuffer(currentFrame);

    constexpr vk::PipelineStageFlags waitStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    const vk::SubmitInfo submitInfo(1, &semaphoreImageAvailable, &waitStageMask,
                                    1, &commandBuffer[currentBufferIdx],
                                    1, &semaphoreEndRendering);
    queueGraphics.submit(submitInfo, fence[currentFrame]);
    submitFrame(); //presentKHR ==> resizeWnd on vk::OutOfDateKHRError and also on vk::Result::eSuboptimalKHR with stderr message
}

void vkApp::resizeWnd()
{
    // get new surface size
    framework.getFramebufferSize((int *)&width, (int *)&height); // e.g. glfwGetFramebufferSize / SDL_GetWindowSize | SDL_Vulkan_GetDrawableSize(
    rebuildAllSwapchain();  // rebuild all swapchain components
    setPerspective();       // recalibrate perspective aspect ratio

// vGizmo3D: is necessary to call when resize window/surface: re-calibrate drag rotation & auto-set mouse sensitivity
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    vgTrackball.viewportSize(width, height);
}

vec3 getLightPosFromQuat(quat &q, float centerDistance) { return (q * vec3(-1.0f, 0.0f, 0.0f)) * centerDistance ;}
quat getQuatRotFromVec3(vec3 &lPos) {
    return normalize(angleAxis(acosf(-lPos.x/length(lPos)), normalize(vec3(FLT_EPSILON, lPos.z, -lPos.y))));
}

void vkApp::run()
{
    // initialize scene: lookAt, perspective, objs: size & pos
    setScene();

    // ImGui Vulkan initialization
    imguiInit();

/// vGizmo3D (3D screen manipulator) initialize:
/// set/associate mouse BUTTON IDs and KEY Modifier IDs to vGizmo3D functionalities
    framework.initVGizmo3D(vgTrackball);

    // imGuIZMO: set mouse feeling and key mods
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    imguiGizmo::setGizmoFeelingRot(.75f);          // default 1.0, >1 more mouse sensitivity, <1 less mouse sensitivity
    imguiGizmo::setPanScale(.5f);                  // default 1.0, >1 more, <1 less
    imguiGizmo::setDollyScale(.5f);                // default 1.0, >1 more, <1 less
    imguiGizmo::setDollyWheelScale(.5f);           // default 1.0, > more, < less ... (from v3.1 separate values)
    imguiGizmo::setPanModifier(vg::evSuperModifier);        // change KEY modifier: CTRL (default) ==> SUPER
    imguiGizmo::setDollyModifier(vg::evControlModifier);    // change KEY modifier: SHIFT (default) ==> CTRL

    // Main render loop
    while(framework.pollEvents()) {     // glfwPollEvents | SDL_PollEvent ... with exit/quit check

/// vGizmo3D: check changing button state to activate/deactivate drag movements
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        framework.checkVGizmo3DMouseEvent(vgTrackball);

// vGizmo3D: call it every rendering loop if you want a continue rotation until you do not click on screen
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        vgTrackball.idle(); // set continuous rotation on Idle: the slow rotation depends on speed of last mouse movement
                            // It can be adjusted from setIdleRotSpeed(1.0) > more speed, < less
                            // It can be stopped by click on screen (without mouse movement)
        vgTrackball.idleSecond();  // also for "secondary" rotation


    // ImGUI: prepare ImGUI new frame
        ImGui_ImplVulkan_NewFrame();
        framework.imguiNewFrame();      // ImGui_ImplGlfw_NewFrame | ImGui_ImplSDL2_NewFrame | ImGui_ImplSDL3_NewFrame
        ImGui::NewFrame();

    // ImGui: Your windows here


    // using vec3 (lightPos) is necessary sync with vGizmo3D : in next example (08) this will no longer be necessary
        lightPos = getLightPosFromQuat(vgTrackball.refSecondRot() ,length(lightPos)); //to syncronize trackball & lightPos passed to the Widgets call

    // Render ALL ImGuIZMO_quat widgets
        renderWidgets(vgTrackball, lightPos, width, height); // in next example (08) we will use directly quaternions

    // using vec3 (lightPos) is necessary re-sync with vGizmo3D: in next example (08) this will no longer be necessary
        vgTrackball.setSecondRot(getQuatRotFromVec3(lightPos));   //to re-syncronize trackball & lightPos passed to the Widgets call


    // transferring the rotation to cube model matrix...
        mat4 modelMatrix = cubeObj * mat4_cast(vgTrackball.getRotation());

    // Build a "translation" matrix
        mat4 translationMatrix = translate(mat4(1), vgTrackball.getPosition());      // add translations (pan/dolly) to an identity matrix

    // old transformations used in "easy_examples": I comment and leave them to make less difficult the reading of the next steps
        //uboMat.mvpMatrix   = clipMatrix * projMatrix * viewMatrix * compensateView * translationMatrix * modelMatrix  ;
        //uboMat.lightMatrix = clipMatrix * projMatrix * viewMatrix * compensateView * translationMatrix * (static_cast<mat4>(vgTrackball.getSecondRot())) * lightObj;

        // Decomposition of the various transformations to use (in different way) with normal, vtx position and light
        uboMat.projMatrix  = clipMatrix * projMatrix ;
        uboMat.viewMatrix  = viewMatrix ;
        uboMat.compMatrix  = compensateView;
        uboMat.modelMatrix = translationMatrix * modelMatrix;
        uboMat.lightMatrix = translationMatrix * static_cast<mat4>(vgTrackball.getSecondRot()) * lightObj;

        // fill fragment uniforms:  get PointOfView (camera position) from viewMatrix...
        uboFrag.PoV = viewMatrix[3];   // in this example PoV does not change, so it would be useless to update it anytime...
        
        // some way to get light position:
        uboFrag.lightPos = uboMat.lightMatrix * vec4(1);  // from LightMatrix
        // another way to get lightPos:
        // light has orbit invariant around cube, of ray always length(lightPos), so...
        // uboFrag.lightPos = getLightPosFromQuat(vgTrackball.refSecondRot(),length(lightPos)) + vgTrackball.getPosition();

        // update uniform buffers to GPU
        for(auto &ubo : uboSceneMat) ubo.update();

    // ImGui: Render/draw
        ImGui::Render();
    // draw the cube, passing matrices to the vtx shader
        draw();             // Render framebuffer
    }

    // Imgui cleanup
    imguiExit();
}

void vkApp::imguiInit()
{
    // Initialize ImGui
        const std::array pool_sizes { vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, 1) } ; // array declaration C++ 17
        vk::DescriptorPoolCreateInfo pool_info(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, //flags
                                               1 /* .maxSets */, pool_sizes.size(), pool_sizes.data() /*, .pNext = nullptr  */ );

        vk::detail::resultCheck(logicalDevice.createDescriptorPool( &pool_info, nullptr, &imguiPool), "ImGui: createDescripptorPool...");

    // Setup/Initialize Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        ImGui::StyleColorsDark(); // ImGui style: or ImGui::StyleColorsLight();

        // Setup Platform/Renderer backends
        framework.imguiInitForVulkan(true);  // ImGui_ImplSDL2_InitForVulkan | ImGui_ImplSDL3_InitForVulkan | ImGui_ImplGLFW_InitForVulkan
                                             // callback ImGui active: you don't even need to handle mouse events
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = instance;
        init_info.PhysicalDevice = physicalDevice;
        init_info.Device = logicalDevice;
        init_info.QueueFamily = graphQueueFamilyIdx;
        init_info.Queue = queueGraphics;
        init_info.PipelineCache = pipelineCache;
        init_info.DescriptorPool = imguiPool;
        init_info.RenderPass = renderPass;                   // Ignored if using dynamic rendering
        init_info.UseDynamicRendering = false;
        init_info.Subpass = 0;
        init_info.MinImageCount = minImageCount;
        init_info.ImageCount = swapChainImages.size();
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.Allocator = nullptr;
        init_info.CheckVkResultFn = nullptr;
        ImGui_ImplVulkan_Init(&init_info);

        ImGui_ImplVulkan_SetMinImageCount(minImageCount);
}

void vkApp::imguiExit() {
    ImGui_ImplVulkan_Shutdown();
    framework.imguiShutdown();      // SDL: ImGui_ImplSDL2/3_Shutdown() | GLFW: ImGui_ImplGlfw_Shutdown()
    ImGui::DestroyContext();
    logicalDevice.destroyDescriptorPool( imguiPool );  // destroy ImGui DescriptorPool

}

void vkApp::onInit()        // called from constructor @ startup
{
    // specific App Vulkan Initializations

    // Send cube Vertex to GPU
    vtxCubeData.init(physicalDevice, logicalDevice, vk::BufferUsageFlagBits::eVertexBuffer);
    vtxCubeData.copyToDevice();

    for(auto &ubo : uboSceneMat) { // uniform buffers for Vtx & Frag shaders
        ubo.init(physicalDevice, logicalDevice, vk::BufferUsageFlagBits::eUniformBuffer);
        ubo.update();
    }
    updateDescriptorsSets(uboSceneMat);

/// vGizmo3D initialize:
// Set Scene, vGizmo3D init and set starting rotations (primary & secondary)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    setScene();

    // If you need to flip "mouse movements" 
    // you can set they in the code or set as default values in vGizmo3D_config.h
    // view in the end of vkApp header file
    APP_FLIP_ROT_X
    APP_FLIP_ROT_Y
    APP_FLIP_ROT_Z
    APP_FLIP_PAN_X
    APP_FLIP_PAN_Y
    APP_FLIP_ROT_X
    APP_FLIP_DOLLY

    APP_REVERSE_AXES
}

void vkApp::onExit()        // called from destructor @ exit
{
    // wait GPU to idle before destroy resources
    logicalDevice.waitIdle();
    // Cleanup
    // Vulkan App specific cleanup
    vtxCubeData.destroy();
    for(auto ubo : uboSceneMat) ubo.destroy();
}

vkApp* vkApp::theMainApp = nullptr;
int main() {
    theApp = new vkApp;     // theApp ==> vkApp::theMainApp
    try { theApp->run(); } catch(const std::exception& e) { std::cerr << e.what() << std::endl; return EXIT_FAILURE; }

    delete theApp;
    return EXIT_SUCCESS;
}