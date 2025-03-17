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
class vkApp;
#define theApp vkApp::theMainApp

#include <limits>

#include "utils/dbgValidationLayer.h"
#include "utils/framework.h"

#include "assets/cubePNC.h"
#include <imguizmo_quat.h>

#define VSYNC_ENABLED true         // true/false vSync on/off ==> pass from FiFo to Immediate presentation mode

static const char *appName = "vkApp";         //Default App name
static uint32_t height = 800, width = 1280;   //Default window size (for this example) 

VULKAN_HPP_INLINE uint32_t getImageCount( const uint32_t requiredCount, const uint32_t minImgCount, const uint32_t maxImgCount )
    { return maxImgCount > 0 ? std::min(std::max(requiredCount, minImgCount), maxImgCount) : std::max(requiredCount, minImgCount); }

class bufferSet {
public:
    bufferSet(void *buffer, vk::DeviceSize size) : bufferPtr(buffer), bufferSize(size) {}

    void destroy() {
        if(logicalDevice) {
            logicalDevice->freeMemory(devMemBuffer);    // Free memory and destroy UBO
            logicalDevice->destroy(buffer);
        }
    }

    uint32_t getMemoryTypeIdx(vk::PhysicalDevice &phyDev, const uint32_t typeFilter, const vk::MemoryPropertyFlags properties) {
        const vk::PhysicalDeviceMemoryProperties memProperties = phyDev.getMemoryProperties();
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)  return i;
        throw std::runtime_error("failed suitable memoType...");
    }

    void init(vk::PhysicalDevice &phyDev, vk::Device &device, vk::BufferUsageFlagBits buffFlags,
                                                              vk::MemoryPropertyFlags memFlags = vk::MemoryPropertyFlagBits::eHostVisible |
                                                                                                 vk::MemoryPropertyFlagBits::eHostCoherent) {
        logicalDevice = &device;
        buffer = device.createBuffer(vk::BufferCreateInfo({} , bufferSize, buffFlags));

        const vk::MemoryRequirements memReq = device.getBufferMemoryRequirements(buffer);
        const vk::MemoryAllocateInfo allocInfo(memReq.size, getMemoryTypeIdx(phyDev, memReq.memoryTypeBits, memFlags));
        devMemBuffer = device.allocateMemory(allocInfo);

        device.bindBufferMemory(buffer, devMemBuffer, 0);
    }

    void update() const {
        assert(logicalDevice!=nullptr);

        void *pData = logicalDevice->mapMemory(devMemBuffer, 0, bufferSize, vk::MemoryMapFlags(0));
        memcpy(pData, bufferPtr, bufferSize);
        logicalDevice->unmapMemory(devMemBuffer); // Unmap after data has been copied
    }

    void copyToDevice(vk::DeviceSize stride = 0 ) {
        assert(logicalDevice!=nullptr);
        uint8_t * deviceData = static_cast<uint8_t *>( logicalDevice->mapMemory( devMemBuffer, 0, bufferSize ) );
        if(!stride) memcpy( deviceData, bufferPtr, bufferSize);
        else { // TODO: not yet tested!!!!! (not used here)
            assert(0);
            vk::DeviceSize size = bufferSize / stride;
            for(size_t i = 0; i < bufferSize; i+=stride) {
                memcpy( deviceData, &((uint8_t *)bufferPtr)[i], size );
                deviceData += stride;
            }
      }
      logicalDevice->unmapMemory( devMemBuffer );
    }

    void *bufferPtr;
    vk::DeviceSize bufferSize;
    vk::Buffer buffer;
private:
    vk::DeviceMemory devMemBuffer;
    vk::Device *logicalDevice = nullptr;
};

class imageSet {
public:
    void init(vk::PhysicalDevice const & physicalDevice,
              vk::Device const &         device,
              vk::Format                 format,
              vk::Extent2D const &       extent,
              vk::ImageTiling            tiling,
              vk::ImageUsageFlags        usage,
              vk::ImageLayout            initialLayout,
              vk::MemoryPropertyFlags    memoryProperties,
              vk::ImageAspectFlags       aspectMask )
    {
        imageSet::format = format;

        image = device.createImage(vk::ImageCreateInfo({}, vk::ImageType::e2D,          format, vk::Extent3D( extent, 1 ), 1, 1,
                                                           vk::SampleCountFlagBits::e1, tiling, usage | vk::ImageUsageFlagBits::eSampled,
                                                           vk::SharingMode::eExclusive, {},     initialLayout ));
        uint32_t memoryTypeIndex = ~0;
        [&] (uint32_t typeBits) {           // findMemoryType
            for (uint32_t i = 0; i < physicalDevice.getMemoryProperties().memoryTypeCount; i++) {
                if ((typeBits & 1) && ((physicalDevice.getMemoryProperties().memoryTypes[i].propertyFlags & memoryProperties) == memoryProperties))  {
                    memoryTypeIndex = i;
                    break;
                }
                typeBits >>= 1;
            }
            assert(memoryTypeIndex != uint32_t(~0));
        } (device.getImageMemoryRequirements(image).memoryTypeBits);

        deviceMemory = device.allocateMemory( vk::MemoryAllocateInfo( device.getImageMemoryRequirements( image ).size, memoryTypeIndex ) );
        device.bindImageMemory(image, deviceMemory, 0);

        vk::ImageViewCreateInfo imageViewCreateInfo({}, image, vk::ImageViewType::e2D, format, {}, { aspectMask, 0, 1, 0, 1 });
        imageView = device.createImageView(imageViewCreateInfo);
    }

    void destroy( vk::Device const & device ) {
        device.destroyImageView( imageView );
        device.destroyImage( image );  // should to be destroyed before the bound DeviceMemory is freed
        device.freeMemory( deviceMemory );
    }

    vk::Format       format;
    vk::Image        image;
    vk::DeviceMemory deviceMemory;
    vk::ImageView    imageView;
};

class depthBufferSet : public imageSet
{
public:
    void init( vk::PhysicalDevice const & physicalDevice, vk::Device const & device, vk::Format format, vk::Extent2D const & extent ) {
        imageSet::init( physicalDevice, device, format, extent,
                   vk::ImageTiling::eOptimal,
                   vk::ImageUsageFlagBits::eDepthStencilAttachment,
                   vk::ImageLayout::eUndefined,
                   vk::MemoryPropertyFlagBits::eDeviceLocal,
                   vk::ImageAspectFlagBits::eDepth );
    }
};

class vkAppBase {
protected:
    vkAppBase() {
        CHECK_VALIDATION_LAYER_SUPPORT()
        createInstance();
        BUILD_DEBUG_MESSENGER(instance)
        selectPhysicalDevice();
        initSurface();
        initLogicalDevice();
        builSwapchain();
        buildSwapChainComponents();
        buildRenderPass();
        createPipelineCache();
        //compileShaders();;
        loadSpirVShaders();
        createDescriptorsPool();
        setupDescriptorsSetLayout();
        buildGraphPipeline();
    }

    void buildSwapChainComponents() {
        createSyncItems();
        buildImagesView();
        buildDepthBuffer();
        buildFramebuffer();
        buildCommandBuffers();
    }

    void createInstance();
    // todo: check supported surface
    void initSurface() { framework.getSurface(instance, physicalDevice, &surface); }
    void selectPhysicalDevice();
    void builSwapchain(vk::SwapchainKHR oldSwapChain = {});
    void buildImagesView();
    void initLogicalDevice();
    void compileShaders();
    void loadSpirVShaders();
    void buildDepthBuffer();
    void buildRenderPass();
    void createPipelineCache();
    void buildGraphPipeline();
    void createSyncItems();
    void buildFramebuffer();
    void createDescriptorsPool();
    void setupDescriptorsSetLayout();
    void updateDescriptorsSets(std::vector<bufferSet> &buffer);
    void buildCommandBuffers();
    void setCommandBuffer(uint32_t currentFrame);
    void rebuildAllSwapchain();
    virtual void resizeWnd() = 0;

    bool prepareFrame() { // return ImageIndex
        vk::ResultValue<uint32_t> retVal {{}, {}};

        try { retVal = logicalDevice.acquireNextImageKHR(swapChain, std::numeric_limits<uint64_t>::max(), semaphoreImageAvailable, {}); }
        catch(vk::OutOfDateKHRError &e) { return false; }
        VK_CATCH("failed on NextImage...")
        if(retVal.result != vk::Result::eSuccess && retVal.result != vk::Result::eSuboptimalKHR) {
            throw "Could not acquire the next swap chain image!";
        }
        currentBufferIdx = retVal.value;
        return true;
    }

    void submitFrame() {
        const vk::PresentInfoKHR presentInfo(1, &semaphoreEndRendering, 1, &swapChain, &currentBufferIdx);

        vk::Result present;
        try { present = queuePresent.presentKHR(presentInfo); } catch (vk::OutOfDateKHRError& err) { resizeWnd(); return; } catch (vk::SystemError& err) { throw std::runtime_error("failed presentQueue..."); }
        if(present == vk::Result::eSuboptimalKHR) { std::cerr << "Suboptimal present!" << std::endl; resizeWnd(); return; }
        // wait present is done
        queuePresent.waitIdle();
    }

    void destroySwapChainComponents()
    {
        depthBuffer.destroy(logicalDevice);
        for (const auto framebuffer : scFrameBuffers) logicalDevice.destroyFramebuffer(framebuffer);
        logicalDevice.freeCommandBuffers(commandPool, commandBuffer);

        // Sync objects
        for(auto f : fence) logicalDevice.destroy(f);                 //
        logicalDevice.destroy(semaphoreEndRendering);
        logicalDevice.destroy(semaphoreImageAvailable);
    }

    ~vkAppBase() {
        // wait GPU to idle before destroy resources
        logicalDevice.waitIdle();
        // If not used "Unique" objects declaration, need to destroy them
        // Swapchain & images
        destroySwapChainComponents();
        logicalDevice.destroySwapchainKHR(swapChain);
        // Command and Frame Buffers
        logicalDevice.destroyPipelineCache(pipelineCache);
        logicalDevice.destroyDescriptorPool(descriptorPool);
        logicalDevice.destroyDescriptorSetLayout(descriptorSetLayout);
        logicalDevice.destroyPipeline(pipeline);
        logicalDevice.destroyPipelineLayout(pipelineLayout);

        logicalDevice.destroy();    // logical device

        framework.destroyWindow();  // now we can destroy framework window (before Surface!)

        instance.destroy(surface);  // framework creates surface, but we need to destroy it
        DESTROY_DEBUG_MESSENGER(instance)
        instance.destroy();

        framework.quit();           // now can quit from framework
    }
    // VK Data
    vk::Format surfaceFormat = vk::Format::eB8G8R8A8Unorm;  // preselected simple surface format
    uint32_t vertexStride { sizeof( cubePNC[0] ) };
    std::vector<std::pair<vk::Format, uint32_t>> vertexInputAttributeFormatOffset { { vk::Format::eR32G32B32A32Sfloat, 0 }, { vk::Format::eR32G32B32A32Sfloat, 4*sizeof(float) }, { vk::Format::eR32G32B32A32Sfloat, 8*sizeof(float) } };

    // preselected eImmediate with vsync=off ==> checking eMailbox is available in presentMode list: minor tearing
    vk::PresentModeKHR pmType = VSYNC_ENABLED ? vk::PresentModeKHR::eFifo : vk::PresentModeKHR::eImmediate;
    const uint32_t minImageCount = 2;                       // number of framebuffers

    vk::PhysicalDevice physicalDevice;
    vk::Instance instance;
    vk::SurfaceKHR surface;
    vk::Device logicalDevice;

    vk::RenderPass renderPass;
    vk::Queue queueGraphics, queuePresent;
    vk::ShaderModule fragShaderMod, vertShaderMod;
    vk::Pipeline pipeline;
    vk::PipelineLayout pipelineLayout;
    vk::PipelineCache pipelineCache;
    vk::SwapchainKHR swapChain;
    std::vector<vk::Image> swapChainImages;
    vk::DescriptorSetLayout descriptorSetLayout;
    vk::DescriptorPool descriptorPool;
    std::vector<vk::DescriptorSet> descriptorSets;
    vk::CommandPool commandPool;
    std::vector<vk::CommandBuffer> commandBuffer;
    std::vector<vk::ImageView> scImageView;     //SwapChainIV
    std::vector<vk::Framebuffer> scFrameBuffers; //SwapChainFB
    vk::Semaphore semaphoreImageAvailable {}, semaphoreEndRendering {};
    std::vector<vk::Fence> fence;
    depthBufferSet depthBuffer;

    struct sharingModeStruct {
        vk::SharingMode sharingMode;
        uint32_t idxCount;
        uint32_t *idxDataPtr;
    } sm;

    debugMessengersLayers debug; // debug messenger obj
    // framework init with predefinite Window size and title
    frameworkApp framework {this, (int)width, (int)height, "vkApp"};

    vk::Extent2D swapChainExtent;
    uint32_t graphQueueFamilyIdx;
    uint32_t currentBufferIdx {0};

    bufferSet vtxCubeData { (void *) cubePNC, sizeof(cubePNC) };
};

// Vulkan App
/////////////////////////////////////////////
class vkApp final : public vkAppBase {
public:
    static vkApp* theMainApp;
    vkApp() { theMainApp = this; onInit(); }
    ~vkApp() { onExit(); }
    void run();

private:
    void draw();
    void resizeWnd();

    void onInit();
    void onExit();

    void imguiInit();
    void imguiExit();
    vk::DescriptorPool imguiPool;

    void setPerspective();
    void setScene();

    /////////////////////////////////////////////
    // App Data
    struct _uboMat {
        mat4 projMatrix;
        mat4 viewMatrix;
        mat4 modelMatrix;
        mat4 compMatrix;
        mat4 lightMatrix;
    } uboMat;

    struct _uboFrag {
        alignas(4*sizeof(float)) vec3 lightPos;
        alignas(4*sizeof(float)) vec3 PoV;
    } uboFrag;


// I maintain this "old" matrices (from easy_examples) to better show the transformations steps and UBO assignments

    mat4 viewMatrix, projMatrix;
    mat4 lightObj, cubeObj;
    mat4 compensateView; // compensate rotation of viewMatrix lookAt Matrix

    std::vector<bufferSet> uboSceneMat {{ (void *) &uboMat,  sizeof(_uboMat)  },
                                        { (void *) &uboFrag, sizeof(_uboFrag) } };

    /// imGuIZMO / vGizmo3D : declare global/static/member/..
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    vg::vGizmo3D vgTrackball; // using vGizmo3D global/static/member instead of specifics variables...
                              // have rotations & Pan/Dolly position variables inside to use with imGuIZMO.quat
                              // And it's necessary if you want use also direct-screen manipulator
};


//////////////////////////////////////////////////////////////////////
// Possible 3D (clipMatrix) spaces ==> Use ONLY ONE of the following

#define APP_HAS_NEG_Y
#if defined(APP_HAS_NEG_Y)
    mat4 clipMatrix = mat4(1.0f,  0.0f, 0.0f, 0.0f,
                           0.0f, -1.0f, 0.0f, 0.0f,
                           0.0f,  0.0f,  .5f, 0.0f,
                           0.0f,  0.0f,  .5f, 1.0f );  // vulkan clip space: -y & half z

    // +Z set depthBufferCompareOp = vk::CompareOp::eLessOrEqual    and depthBufferClearValue = 1.0
    vk::CompareOp depthBufferCompareOp = vk::CompareOp::eLessOrEqual;  // of vk::PipelineDepthStencilStateCreateInfo
    const float depthBufferClearValue  = 1.0;                          // "depth" value of vk::ClearDepthStencilValue

    #define PERSPECTIVE perspectiveRH_ZO // RightHanded space and Z-Buffer [0,1] ==> default "prespective" (both GLM/vgMath) is set to perspectiveRH_NO
    #define LOOK_AT     lookAt           // default "looAt" (both GLM/vgMath) is set to lookAtRH ==> !!! you can change ALL defaults in vgMath_config or in GLM (if use it) !!!
    // the defautl values can be changed in vgConfig.h

    // No rotation changes
    #define APP_FLIP_ROT_X
    #define APP_FLIP_ROT_Y
    #define APP_FLIP_ROT_Z
    #define APP_FLIP_PAN_X
    #define APP_FLIP_ROT_Y
    #define APP_FLIP_PAN_Y
    #define APP_FLIP_DOLLY
    #define APP_REVERSE_AXES
#endif

//#define APP_HAS_NEG_YZ
#if defined(APP_HAS_NEG_YZ)
    mat4 clipMatrix = mat4(1.0f,  0.0f, 0.0f, 0.0f,
                           0.0f, -1.0f, 0.0f, 0.0f,
                           0.0f,  0.0f,-1.0f, 0.0f,
                           0.0f,  0.0f, 1.0f, 1.0f );  // vulkan clip space: -y & -z

    #define PERSPECTIVE perspectiveRH_ZO // RightHanded space and Z-Buffer [0,1] ==> default "prespective" (both GLM/vgMath) is set to perspectiveRH_NO
    #define LOOK_AT     lookAt             // default "looAt" (both GLM/vgMath) is set to lookAtRH ==> !!! you can change ALL defaults in vgMath_config or in GLM (if use it) !!!
    // the defautl values can be changed vgMath_config (vgMath) or in GLM (if use it) !!!

    // -Z set depthBufferCompareOp = vk::CompareOp::eGreaterOrEqual and depthBufferClearValue = 0.0
    vk::CompareOp depthBufferCompareOp = vk::CompareOp::eGreaterOrEqual;    // of vk::PipelineDepthStencilStateCreateInfo
    const float depthBufferClearValue  = 0.0;                               // "depth" value of vk::ClearDepthStencilValue

    // No rotation changes
    #define APP_FLIP_ROT_X
    #define APP_FLIP_ROT_Y
    #define APP_FLIP_ROT_Z
    #define APP_FLIP_PAN_X
    #define APP_FLIP_PAN_Y
    #define APP_FLIP_DOLLY
    #define APP_REVERSE_AXES
#endif


    //////////////////////////////////////////////////////////////////////
    // A possible vulkan "untouched" 3D space ==> with identity clipMatrix (therefore no changes)
    // lightPosition is always: vec3(2, 2.5, 3) ... but:
    // The "cube light spot" is DOWN respect MainCube centered in (0,0,0) ==> Y grows down
    // It's also behind the MainCube ==> Z grows forward
    // reversing Y axis change also the "apparent" rotation (toward from POV)

//#define APP_VULKAN_NATIVE
#if defined(APP_VULKAN_NATIVE)

    mat4 clipMatrix = mat4(1.0f);  // vulkan clip space: NO changes ==> identity Matrix

    // +Z set depthBufferCompareOp = vk::CompareOp::eLessOrEqual    and depthBufferClearValue = 1.0
    vk::CompareOp depthBufferCompareOp = vk::CompareOp::eLessOrEqual;  // of vk::PipelineDepthStencilStateCreateInfo
    const float depthBufferClearValue  = 1.0;                          // "depth" value of vk::ClearDepthStencilValue

    #define PERSPECTIVE perspectiveLH_ZO // LeftHanded space and Z-Buffer [0,1] ==> default "prespective" is set to perspectiveRH_NO Z-Buffer [-1,1]
    #define LOOK_AT     lookAtLH         // LeftHanded space ==> default "looAt" is set to lookAtRH ==> !!! you can change defaults in vGizmo3D_config !!!
    // the defautl values can be changed in vgConfig.h


    // And is also necessary override some rotation:
    // reversing Y axis change also the "apparent" rotation (toward from POV)
    // and obviously also the all the Y drag: also Dolly (in/out) is influenced from mouse Y (up/douw)

    // ALL these settings can be made in the code or (una tantum) set as default in vGizmo3D_config.h (if use it)
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    // All possible settings: These override the vGizmo3D_config.h ones: "get" current setting and "revert" it
    #define APP_FLIP_ROT_X //vgTrackball.flipRotOnY(!vgTrackball.getFlipRotOnY());
    #define APP_FLIP_ROT_Y vgTrackball.flipRotOnY(!vgTrackball.getFlipRotOnY());
    #define APP_FLIP_ROT_Z vgTrackball.flipRotOnZ(!vgTrackball.getFlipRotOnZ());

    #define APP_FLIP_PAN_X //vgTrackball.setFlipPanY(!vgTrackball.getFlipPanY());
    #define APP_FLIP_PAN_Y vgTrackball.setFlipPanY(!vgTrackball.getFlipPanY());
    #define APP_FLIP_DOLLY vgTrackball.setFlipDolly(!vgTrackball.getFlipDolly());

#endif
