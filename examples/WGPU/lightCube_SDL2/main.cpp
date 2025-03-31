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
#include <cstdio>
#include <cassert>
#include <cfloat>
/////////////////////////////////////////////////////////////////////////////
// imGuIZMO: include imGuIZMOquat.h or imguizmo_quat.h
#include <imguizmo_quat.h> // now also imguizmo_quat.h from v3.1

#include "imgui_impl_sdl2.h"
#include "imgui_impl_wgpu.h"

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #include <emscripten/html5.h>
    #include <emscripten/html5_webgpu.h>
    #define DEVICE_TICK
#else
    // Tick needs to be called in Dawn to display validation errors
    #define DEVICE_TICK  device.Tick();
#endif

#define SDL_MAIN_HANDLED
#include "utils/sdl2wgpu.h"
#include <webgpu/webgpu.h>
#include <webgpu/webgpu_cpp.h>

#include "assets/cubePNC.h"

void renderWidgets(vg::vGizmo3D &track, vec3& vLight, int width, int height);

// WGSL shader
const char *shader  = {
    #include "shaders/cube_light.wgsl"
};

/////////////////////////////////////////////
// App Data

/// imGuIZMO / vGizmo3D : declare global/static/member/..
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
vg::vGizmo3D vgizmo;     // using vGizmo3D global/static/member instead of specifics variables...
// have rotations & Pan/Dolly position variables inside to use with imGuIZMO.quat
// And it's necessary if you want use also direct-screen manipulator

struct uboData {
    mat4 projMatrix;
    mat4 viewMatrix;
    mat4 modelMatrix;
    mat4 compMatrix;
    mat4 lightMatrix;
    alignas(sizeof(vec4)) vec3 lightPos;
    alignas(sizeof(vec4)) vec3 PoV;
} uboMat;

// I maintain this "old" matrices (from easy_examples) to better show the transformations steps and UBO assignments
mat4 mvpMatrix, viewMatrix, projMatrix;
mat4 lightObj, lightMatrix, cubeObj;
mat4 compensateView; // compensate rotation of viewMatrix lookAt Matrix

vec3 lightPos(2, 2.5, 3);        // Light Position


// Initial App state
static const uint32_t initialWindowWidth {1280};
static const uint32_t initialWindowHeight {800};
static const char *appTitle {"wgpu - Mandelbrot - SDL2 example"};

// Global WebGPU required
wgpu::Instance              instance;
wgpu::Device                device;
wgpu::Surface               surface;
wgpu::TextureFormat         preferredFormat { wgpu::TextureFormat::Undefined };  // current undefined, but set from SurfaceCapabilities
wgpu::SurfaceConfiguration  surfaceConfig;

// Pipeline related objs
wgpu::RenderPipeline pipeline;
wgpu::Buffer ubo;
wgpu::BindGroupLayout bindGroupLayout;

wgpu::Buffer vertexBuffer;
wgpu::Texture     depthTexture;
wgpu::TextureView depthTextureView;
const wgpu::TextureFormat depthTextureFormat = wgpu::TextureFormat::Depth32Float;

// SDL2 main framework window
SDL_Window* fwWindow = nullptr;

// helper func: vec3 <===> quat  (vec3 is center axes based)
vec3 getLightPosFromQuat(quat &q, float centerDistance) { return (q * vec3(-1.0f, 0.0f, 0.0f)) * centerDistance ;}
quat getQuatRotFromVec3(vec3 &lPos) {
    return normalize(angleAxis(acosf(-lPos.x/length(lPos)), normalize(vec3(FLT_EPSILON, lPos.z, -lPos.y))));
}


void setPerspective(int width, int height)
{
    float aspectRatio = float(height) / float(width);       // Set "camera" position and perspective
    float fov = radians( 45.0f ) * aspectRatio;
    projMatrix = perspective( fov, 1.f/aspectRatio, 0.1f, 100.0f );
}

void setScene()
{
    viewMatrix =  lookAt( { 12.0f,  6.0f,  4.0f },   // From / EyePos / PoV
                          {  0.0f,  0.0f,  0.0f },   // To   /  Tgt
                          {  3.0f,  1.0f,   .0f } ); // Up

    // Now scale cube to better view light position
    cubeObj = mat4(1); // nothing to do ... scale( vec3(.5));

/// imGuIZMO / vGizmo3D
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //vgizmo.setRotation(quat(1,0,0,0));                     // vGizmo3D with NO initial rotation (default initialization)
    //vgizmo.setRotation(eulerAngleXYZ(vec3(radians(45),
    //                                     radians( 0),
    //                                     radians( 0))));  // vGizmo3D with rotation of 45 degrees on X axis
    vgizmo.setRotation(viewMatrix);                          // vGizmo3D with ViewMatrix (lookAt) rotation

    // for Pan & Dolly always bounded on screen coords (x = left/right, y = up/douw, z = in/out) we remove viewMatrix rotation
    // otherwise Pan & Dolly have as reference the Cartesian axes
    compensateView = inverse(mat4_cast(quat(viewMatrix)));

    // light model

    // acquiring rotation for the light pos
    const float len = length(lightPos);
     //if(len<1.0 && len>= FLT_EPSILON) { normalize(lightPos); len = 1.0; }  // controls are not necessary: lightPos is known
     //else if(len > FLT_EPSILON)
        quat q = angleAxis(acosf(-lightPos.x/len), normalize(vec3(FLT_EPSILON, lightPos.z, -lightPos.y)));
    vgizmo.setSecondRot(q);          // store secondary rotation for the Light

    lightObj = translate(mat4(1), lightPos);
    lightObj = scale(lightObj, vec3(.1));       // using same cube vertex but with 10% size

    lightObj = inverse(static_cast<mat4>(vgizmo.getSecondRot())) * lightObj ;

    setPerspective(initialWindowWidth, initialWindowHeight);
}

/// vGizmo3D initialize: <br>
/// set/associate mouse BUTTON IDs and KEY modifier IDs to vGizmo3D functionalities <br><br>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void initVGizmo3D()     // Settings to control vGizmo3D
{
    // Initialization is necessary to associate your preferences to vGizmo3D
    // These are also the DEFAULT values, so if you want to maintain these combinations you can omit they
    // and to override only the associations that you want modify
        vgizmo.setGizmoRotControl         (vg::evButton1  /* or vg::evLeftButton */, 0 /* vg::evNoModifier */ );
    // Rotations around specific axis: mouse button and key modifier
        vgizmo.setGizmoRotXControl        (vg::evButton1  /* or vg::evLeftButton */, vg::evShiftModifier);
        vgizmo.setGizmoRotYControl        (vg::evButton1  /* or vg::evLeftButton */, vg::evControlModifier);
        vgizmo.setGizmoRotZControl        (vg::evButton1  /* or vg::evLeftButton */, vg::evAltModifier | vg::evSuperModifier);
    // Set vGizmo3D control for secondary rotation
        vgizmo.setGizmoSecondRotControl(vg::evButton2  /* or vg::evRightButton */, 0 /* vg::evNoModifier */ );
    // Pan and Dolly/Zoom: mouse button and key modifier
        vgizmo.setDollyControl            (vg::evButton2 /* or vg::evRightButton */, vg::evControlModifier);
        vgizmo.setPanControl              (vg::evButton2 /* or vg::evRightButton */, vg::evShiftModifier);
    // N.B. vg::enums are ONLY mnemonic: select and pass specific vg::enum to framework (that can have also different IDs)

    // passing the screen sizes calibrate drag rotation and auto-set the mouse sensitivity
        vgizmo.viewportSize(initialWindowWidth, initialWindowWidth);      // is necessary also to call when resize window/surface: re-calibrate drag rotation & auto-set mouse sensitivity
    // vgizmo.setGizmoFeeling(1.0);              // but if you need to more feeling with the mouse use: 1.0 default,  > 1.0 more sensible, < 1.0 less sensible

    // setIdleRotSpeed(1.0)                     // If used Idle() feature (continue rotation on Idle) it set that speed: more speed > 1.0 ,  less < 1.0

    // other settings if you need it
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // vgizmo.setDollyScale(1.0f);               // > 1.0 more sensible, < 1.0 less sensible
    // vgizmo.setDollyPosition(/* your pos */);  // input: float/double or vec3... in vec3 only Z is acquired
    // vgizmo.setPanScale(1.0f);                 // > 1.0 more sensible, < 1.0 less sensible
    // vgizmo.setPanyPosition(/* your pos */);   // vec3 ==> only X and Y are acquired
    // vgizmo.setPosition(/* your pos */);       // input vec3 is equivalent to call: vgizmo.setDollyPosition(/* your pos */); and vgizmo.setPanyPosition(/* your pos */);
    // vgizmo.setRotationCenter(/* vec3 */);     // new rotation center
    //
    // Watch vGizmo.h for more functionalities
}

// WGPU VL callbacks
#if !defined(__EMSCRIPTEN__)
static void wgpu_device_lost_callback(const wgpu::Device&, wgpu::DeviceLostReason reason, wgpu::StringView message)
{
    const char* reasonName = "";
    switch (reason) {
        case wgpu::DeviceLostReason::Unknown:         reasonName = "Unknown";         break;
        case wgpu::DeviceLostReason::Destroyed:       reasonName = "Destroyed";       break;
        case wgpu::DeviceLostReason::CallbackCancelled: reasonName = "InstanceDropped"; break;
        case wgpu::DeviceLostReason::FailedCreation:  reasonName = "FailedCreation";  break;
        default:                                      reasonName = "UNREACHABLE";     break;
    }
    printf("%s device message: %s\n", reasonName, message.data);
}

static void wgpu_error_callback(const wgpu::Device&, wgpu::ErrorType type, wgpu::StringView message)
{
    const char* errorTypeName = "";
    switch (type) {
        case wgpu::ErrorType::Validation:  errorTypeName = "Validation";      break;
        case wgpu::ErrorType::OutOfMemory: errorTypeName = "Out of memory";   break;
        case wgpu::ErrorType::Unknown:     errorTypeName = "Unknown";         break;
        case wgpu::ErrorType::Internal:    errorTypeName = "Internal";        break;
        default:                           errorTypeName = "UNREACHABLE";     break;
    }
    printf("%s error: %s\n", errorTypeName, message.data);
}

void initWGPU()
{
    wgpu::InstanceDescriptor instanceDescriptor;
    instanceDescriptor.capabilities.timedWaitAnyEnable = true;
    instance = wgpu::CreateInstance(&instanceDescriptor);

    static wgpu::Adapter localAdapter;
    wgpu::RequestAdapterOptions adapterOptions;

    auto onRequestAdapter = [](wgpu::RequestAdapterStatus status, wgpu::Adapter adapter, wgpu::StringView message) {
        if (status != wgpu::RequestAdapterStatus::Success) {
            printf("Failed to get an adapter: %s\n", message.data);
            return;
        }
        localAdapter = std::move(adapter);
    };

    // Synchronously (wait until) acquire Adapter
    auto waitedAdapterFunc { instance.RequestAdapter(&adapterOptions, wgpu::CallbackMode::WaitAnyOnly, onRequestAdapter) };
    auto waitStatus = instance.WaitAny(waitedAdapterFunc, UINT64_MAX);
    assert(localAdapter != nullptr && waitStatus == wgpu::WaitStatus::Success && "Error on Adapter request");

#ifndef NDEBUG
    wgpu::AdapterInfo info;
    localAdapter.GetInfo(&info);
    printf("Using adapter: \" %s \"\n", info.device.data);
#endif

    // Set device callback functions
    wgpu::DeviceDescriptor deviceDesc;
    deviceDesc.SetDeviceLostCallback(wgpu::CallbackMode::AllowSpontaneous, wgpu_device_lost_callback);
    deviceDesc.SetUncapturedErrorCallback(wgpu_error_callback);

    // get device Synchronously
    device = localAdapter.CreateDevice(&deviceDesc);
    assert(device != nullptr && "Error creating the Device");

    surface = wgpu::Surface(SDL_getWGPUSurface(instance.Get(), fwWindow));
    assert(surface != nullptr && "Error creating the Surface");

    // Configure the surface.
    wgpu::SurfaceCapabilities capabilities;
    surface.GetCapabilities(localAdapter, &capabilities);
    preferredFormat = capabilities.formats[0];

    surfaceConfig.device          = device;
    surfaceConfig.format          = preferredFormat;
    surfaceConfig.usage           = wgpu::TextureUsage::RenderAttachment;
    surfaceConfig.width           = initialWindowWidth;
    surfaceConfig.height          = initialWindowHeight;
    surfaceConfig.alphaMode       = wgpu::CompositeAlphaMode::Auto;
    surfaceConfig.presentMode     = wgpu::PresentMode::Fifo;

    surface.Configure(&surfaceConfig);
}
#else
// Adapter and device initialization via JS
EM_ASYNC_JS( void, getAdapterAndDeviceViaJS, (),
{
    if (!navigator.gpu) throw Error("WebGPU not supported.");

    const adapter = await navigator.gpu.requestAdapter();
    const device = await adapter.requestDevice();
    Module.preinitializedWebGPUDevice = device;
} );

void initWGPU()
{
    getAdapterAndDeviceViaJS();

    instance = wgpu::CreateInstance(nullptr);
    device   = wgpu::Device(emscripten_webgpu_get_device());
    assert(device != nullptr && "Error creating the Device");

    wgpu::SurfaceDescriptorFromCanvasHTMLSelector html_surface_desc;
    html_surface_desc.selector = "#canvas";
    wgpu::SurfaceDescriptor surface_desc;
    surface_desc.nextInChain   = &html_surface_desc;

    surface         = instance.CreateSurface(&surface_desc);
    preferredFormat = surface.GetPreferredFormat({} /* adapter */);

    surfaceConfig.device = device;
    surfaceConfig.format = preferredFormat;

}
#endif

// Initialize render pipeline
void initRenderPipeline()
{
#if defined(__EMSCRIPTEN__)
    wgpu::ShaderModuleWGSLDescriptor wgslDesc;
    wgslDesc.code = shader;
                                                //  format                         offset          location
    std::array attributes { wgpu::VertexAttribute {  wgpu::VertexFormat::Float32x4, 0             , 0 },
                            wgpu::VertexAttribute {  wgpu::VertexFormat::Float32x4, sizeof(vec4)  , 1 },
                            wgpu::VertexAttribute {  wgpu::VertexFormat::Float32x4, sizeof(vec4)*2, 2 } };
#else
    wgpu::ShaderSourceWGSL wgslDesc;
    wgslDesc.code = { shader, WGPU_STRLEN };
                                                // chain    format                         offset          location
    std::array attributes { wgpu::VertexAttribute { nullptr, wgpu::VertexFormat::Float32x4, 0             , 0 },
                            wgpu::VertexAttribute { nullptr, wgpu::VertexFormat::Float32x4, sizeof(vec4)  , 1 },
                            wgpu::VertexAttribute { nullptr, wgpu::VertexFormat::Float32x4, sizeof(vec4)*2, 2 } };
#endif

    // Create buffers

    // Cube VertexBuffer
    wgpu::BufferDescriptor descriptor;
    descriptor.size = sizeof(cubePNC);
    descriptor.usage = wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst;
    vertexBuffer = device.CreateBuffer(&descriptor);
    device.GetQueue().WriteBuffer(vertexBuffer, 0, cubePNC, sizeof(cubePNC));

    wgpu::VertexBufferLayout vertexBufferLayout;
    vertexBufferLayout.arrayStride    = sizeof( vertexPNC );
    vertexBufferLayout.stepMode       = wgpu::VertexStepMode::Vertex;
    vertexBufferLayout.attributeCount = std::size( attributes );
    vertexBufferLayout.attributes     = attributes.data();

    // Uniform Buffer
    wgpu::BufferDescriptor bufferDesc;
    bufferDesc.size = sizeof(uboData);
    bufferDesc.usage = wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Uniform;
    ubo = device.CreateBuffer(&bufferDesc);

    // Depth/Stencil state.
    wgpu::DepthStencilState depthStencilState;
    depthStencilState.format              = depthTextureFormat;
    depthStencilState.depthWriteEnabled   = true;
    depthStencilState.depthCompare        = wgpu::CompareFunction::Less;

    // Shader module
    wgpu::ShaderModuleDescriptor shaderDescriptor;
    shaderDescriptor.nextInChain = &wgslDesc;
    wgpu::ShaderModule module = device.CreateShaderModule(&shaderDescriptor);

    // struct BlendComponent already with default set: src=One, dst=Zero, op=Add
    wgpu::BlendState blend;

    // color target attribs
    wgpu::ColorTargetState colorTarget;
    colorTarget.format = wgpu::TextureFormat(preferredFormat);
    colorTarget.blend = &blend;
    colorTarget.writeMask = wgpu::ColorWriteMask::All;

    // Fragment Shader
    wgpu::FragmentState fragment;
    fragment.module = module;
    fragment.targetCount = 1;
    fragment.targets = &colorTarget;

    // @group(0) @binding(0) var<uniform> shaderData
    wgpu::BindGroupLayoutEntry bindGroupLayoutEntry;
    bindGroupLayoutEntry.binding               = 0;
    bindGroupLayoutEntry.visibility            = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
    bindGroupLayoutEntry.buffer.type           = wgpu::BufferBindingType::Uniform;
    bindGroupLayoutEntry.buffer.minBindingSize = sizeof(uboData);

    // BindGroupLayout
    wgpu::BindGroupLayoutDescriptor bindGroupLayoutDesc;
    bindGroupLayoutDesc.entryCount = 1;
    bindGroupLayoutDesc.entries = &bindGroupLayoutEntry;
    bindGroupLayout = device.CreateBindGroupLayout(&bindGroupLayoutDesc);

    // pipelineLayout
    wgpu::PipelineLayoutDescriptor layoutDesc;
    layoutDesc.bindGroupLayoutCount = 1;
    layoutDesc.bindGroupLayouts = &bindGroupLayout;

    // Render pipeline descriptor
    wgpu::RenderPipelineDescriptor pipelineDesc;
    pipelineDesc.layout = device.CreatePipelineLayout(&layoutDesc);     // wgpu::pipelineLayout
    pipelineDesc.vertex.module = module;
    pipelineDesc.vertex.bufferCount = 1;
    pipelineDesc.vertex.buffers = &vertexBufferLayout;
    pipelineDesc.fragment = &fragment;                                  // wgpu::FragmentState
    pipelineDesc.depthStencil = &depthStencilState;
    // Set primitive state
    pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
    pipelineDesc.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined;
    pipelineDesc.primitive.frontFace = wgpu::FrontFace::CCW;
    pipelineDesc.primitive.cullMode = wgpu::CullMode::None;

    // Create Render Pipeline
    pipeline = device.CreateRenderPipeline(&pipelineDesc);
}

void resizeSurface(const uint32_t width, const uint32_t height)
{
    wgpu::TextureDescriptor depthTextureDesc;
    depthTextureDesc.usage                   = wgpu::TextureUsage::RenderAttachment;
    depthTextureDesc.format                  = wgpu::TextureFormat::Depth32Float;
    depthTextureDesc.viewFormatCount         = 1;
    depthTextureDesc.viewFormats             = &depthTextureFormat;
    depthTextureDesc.size.width              = width;
    depthTextureDesc.size.height             = height;
    // depthTexture: overload operator "=" (assignment) Release left operand (previous object), before to assign
    depthTexture = device.CreateTexture(&depthTextureDesc);

    wgpu::TextureViewDescriptor depthTextureViewDesc;
    depthTextureViewDesc.format          = depthTextureFormat;
    depthTextureViewDesc.dimension       = wgpu::TextureViewDimension::e2D;
    depthTextureViewDesc.mipLevelCount   = 1;
    depthTextureViewDesc.arrayLayerCount = 1;
    depthTextureViewDesc.aspect          = wgpu::TextureAspect::DepthOnly;
    depthTextureDesc.usage               = wgpu::TextureUsage::RenderAttachment;
    // depthTextureView: overload operator "=" (assignment) Release left operand (previous object), before to assign
    depthTextureView = depthTexture.CreateView( &depthTextureViewDesc );

    surfaceConfig.width  = width;
    surfaceConfig.height = height;

    ImGui_ImplWGPU_InvalidateDeviceObjects();

    surface.Configure(&surfaceConfig);

    ImGui_ImplWGPU_CreateDeviceObjects();
}

/// vGizmo3D: Check key modifier currently pressed (SDL2 version)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int getModifier(SDL_Window* window = nullptr) {
    SDL_Keymod keyMod = SDL_GetModState();
    if(keyMod & KMOD_CTRL)          return vg::evControlModifier;
    else if(keyMod & KMOD_SHIFT)    return vg::evShiftModifier;
    else if(keyMod & KMOD_ALT)      return vg::evAltModifier;
    else if(keyMod & KMOD_GUI)      return vg::evSuperModifier;
    else return vg::evNoModifier;
}

void mouseMovement()
{
    // vGizmo3D: check changing button state to activate/deactivate drag movements (pressing together left/right activate/deactivate both)
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if(!ImGui::GetIO().WantCaptureMouse) {
            static int leftPress = 0, rightPress = 0, middlePress = 0;
        int x, y;
        int mouseState = SDL_GetMouseState(&x, &y);
        if(leftPress != (mouseState & SDL_BUTTON_LMASK)) {                                   // check if leftButton state is changed
            leftPress = mouseState & SDL_BUTTON_LMASK ;                                    // set new (different!) state
            vgizmo.mouse(vg::evLeftButton, getModifier(fwWindow),       // send communication to vGizmo3D...
                                          leftPress, x, y);             // ... checking if a key modifier currently is pressed
        }
        if(rightPress != (mouseState & SDL_BUTTON_RMASK)) {                                  // check if rightButton state is changed
            rightPress = mouseState & SDL_BUTTON_RMASK;                                    // set new (different!) state
            vgizmo.mouse(vg::evRightButton, getModifier(fwWindow),      // send communication to vGizmo3D...
                                           rightPress, x, y);           // ... checking if a key modifier currently is pressed
        }
        // Simulating a double press (left+right button) using MIDDLE button,
        // sending two "consecutive" activation/deactivation calls to rotate obj-model and light spot together
        if(middlePress != (mouseState & SDL_BUTTON_MMASK)) {             // check if middleButton state is changed
            middlePress = mouseState & SDL_BUTTON_MMASK;                 // set new (different!) middle button state
            vgizmo.mouse(vg::evRightButton, getModifier(fwWindow), middlePress, x, y);  // call Right activation/deactivation with same "middleStatus"
            vgizmo.mouse(vg::evLeftButton,  getModifier(fwWindow), middlePress, x, y);  // call Left  activation/deactivation with same "middleStatus"
        }
        // vGizmo3D: if "drag" active update internal rotations (primary and secondary)
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        vgizmo.motion(x,y);
    }

    // vGizmo3D: call it every rendering loop if you want a continue rotation until you do not click on screen
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    vgizmo.idle();   // set continuous rotation on Idle: the slow rotation depends on speed of last mouse movement
                    // It can be adjusted from setIdleRotSpeed(1.0) > more speed, < less
                    // It can be stopped by click on screen (without mouse movement)
    vgizmo.idleSecond();
}

void updateMatrices()
{
    // transferring the rotation to cube model matrix...
    mat4 modelMatrix = cubeObj * mat4_cast(vgizmo.getRotation());

    // Build a "translation" matrix
    mat4 translationMatrix = translate(mat4(1), vgizmo.getPosition());      // add translations (pan/dolly) to an identity matrix

    // old transformations used in "easy_examples": I comment and leave them to make less difficult the reading of the next steps
    //uboMat.mvpMatrix   = projMatrix * viewMatrix * compensateView * translationMatrix * modelMatrix  ;
    //uboMat.lightMatrix = projMatrix * viewMatrix * compensateView * translationMatrix * (static_cast<mat4>(vgTrackball.getSecondRot())) * lightObj;

    // Decomposition of the various transformations to use (in different way) with normal, vtx position and light
    uboMat.projMatrix  = projMatrix ;
    uboMat.viewMatrix  = viewMatrix ;
    uboMat.compMatrix  = compensateView;
    uboMat.modelMatrix = translationMatrix * modelMatrix;
    uboMat.lightMatrix = translationMatrix * static_cast<mat4>(vgizmo.getSecondRot()) * lightObj;

    // get PointOfView (camera position) from viewMatrix...
    uboMat.PoV = viewMatrix[3];    // in this example PoV does not change, so it would be useless to update it anytime...

    // some way to get light position:
    uboMat.lightPos = uboMat.lightMatrix * vec4(1);  // from LightMatrix
    // another way to get lightPos:
    // light has orbit invariant around cube, of ray always length(lightPos), so...
    // uboFrag.lightPos = getLightPosFromQuat(vgTrackball.refSecondRot(),length(lightPos)) + vgTrackball.getPosition();

    device.GetQueue().WriteBuffer( ubo, 0, &uboMat, sizeof( uboData ) );
}

void renderImGui()
{
    // Start the Dear ImGui frame
    ImGui_ImplWGPU_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // using vec3 (lightPos) is necessary sync with vGizmo3D : in next example (08) this will no longer be necessary
    lightPos = getLightPosFromQuat(vgizmo.refSecondRot() ,length(lightPos)); //to syncronize trackball & lightPos passed to the Widgets call

    // Render ALL ImGuIZMO_quat widgets
    renderWidgets(vgizmo, lightPos, surfaceConfig.width, surfaceConfig.height); // in next example (08) we will use directly quaternions

    // using vec3 (lightPos) is necessary re-sync with vGizmo3D: in next example (08) this will no longer be necessary
    vgizmo.setSecondRot(getQuatRotFromVec3(lightPos));   //to re-syncronize trackball & lightPos passed to the Widgets call

    // Rendering
    ImGui::Render();
}

WGPUTexture checkSurfaceTextureStatus()
{
    WGPUSurfaceTexture surfaceTexture;
    wgpuSurfaceGetCurrentTexture(surface.Get(), &surfaceTexture);

    switch ( surfaceTexture.status ) {
#if defined(__EMSCRIPTEN__)
        case WGPUSurfaceGetCurrentTextureStatus_Success:
            break;
#else
        case WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal:
            break;
        case WGPUSurfaceGetCurrentTextureStatus_SuccessSuboptimal:
#endif
        case WGPUSurfaceGetCurrentTextureStatus_Timeout:
        case WGPUSurfaceGetCurrentTextureStatus_Outdated:
        case WGPUSurfaceGetCurrentTextureStatus_Lost:
        // if the status is NOT Optimal let's try to reconfigure the surface
        {
#ifndef NDEBUG
            printf("Bad surface texture status: %d\n", surfaceTexture.status);
#endif
            if (surfaceTexture.texture)
                wgpuTextureRelease(surfaceTexture.texture);
            int width, height;
            SDL_GetWindowSize(fwWindow, &width, &height);
            if ( width > 0 && height > 0 )
            {
                surfaceConfig.width  = width;
                surfaceConfig.height = height;

                surface.Configure(&surfaceConfig);
            }
            return nullptr;
        }
        default:            // should never be reached
            return nullptr;
    }
    return surfaceTexture.texture;
}

void mainLoop()
{
    mouseMovement();
    updateMatrices();

    // React to changes in screen size
    int width, height;
    SDL_GetWindowSize(fwWindow, &width, &height);
    if (width != surfaceConfig.width || height != surfaceConfig.height)
    {
        resizeSurface(width, height);

        // vGizmo3D: is necessary to call when resize window/surface: re-calibrate drag rotation & auto-set mouse sensitivity
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        vgizmo.viewportSize(width, height);
        setPerspective(width, height);       // recalibrate perspective aspect ratio
    }

    wgpu::Texture texture = checkSurfaceTextureStatus();
    if(!texture) return;

    renderImGui();

    wgpu::TextureViewDescriptor viewDescriptor;
    viewDescriptor.format          = preferredFormat;
    viewDescriptor.dimension       = wgpu::TextureViewDimension::e2D;

    wgpu::RenderPassColorAttachment colorAttach;
    colorAttach.loadOp     = wgpu::LoadOp::Clear;
    colorAttach.storeOp    = wgpu::StoreOp::Store;
    colorAttach.clearValue = {};                      //wgpu::Color
    colorAttach.view       = texture.CreateView(&viewDescriptor);

    wgpu::RenderPassDepthStencilAttachment depthAttach;
    depthAttach.view              = depthTextureView;
    depthAttach.depthLoadOp       = wgpu::LoadOp::Clear;
    depthAttach.depthStoreOp      = wgpu::StoreOp::Store;
    depthAttach.depthClearValue   = 1.0f;
    depthAttach.depthReadOnly     = false;

    wgpu::RenderPassDescriptor renderPassDesc;
    renderPassDesc.colorAttachmentCount   = 1;
    renderPassDesc.colorAttachments       = &colorAttach;
    renderPassDesc.depthStencilAttachment = &depthAttach;

    wgpu::CommandEncoderDescriptor enc_desc;
    wgpu::CommandEncoder encoder = device.CreateCommandEncoder(&enc_desc);

    wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPassDesc);

    pass.SetPipeline(pipeline);
    pass.SetVertexBuffer(0, vertexBuffer, 0, sizeof(cubePNC));

    // Bind the uniform buffer.
    wgpu::BindGroupEntry bindingEntry;
    bindingEntry.binding = 0;
    bindingEntry.buffer  = ubo;
    bindingEntry.offset  = 0;
    bindingEntry.size    = sizeof( uboData );

    wgpu::BindGroupDescriptor bindGroupDescriptor;
    bindGroupDescriptor.layout     = bindGroupLayout;
    bindGroupDescriptor.entryCount = 1;
    bindGroupDescriptor.entries    = &bindingEntry;
    wgpu::BindGroup bindGroup        = device.CreateBindGroup(&bindGroupDescriptor );

    pass.SetBindGroup(0, bindGroup, 0, nullptr );

    pass.Draw(12 * 3, 2, 0, 0);

    ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), pass.Get()); // add Imgui RenderPass data
    pass.End();

    wgpu::CommandBufferDescriptor cmd_buffer_desc;
    wgpu::CommandBuffer cmd_buffer = encoder.Finish(&cmd_buffer_desc);
    device.GetQueue().Submit(1, &cmd_buffer);

#if !defined(__EMSCRIPTEN__)
    surface.Present();
    device.Tick();
#endif
}

void initImGui()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOther(fwWindow);
    ImGui_ImplWGPU_InitInfo init_info;
    init_info.Device = device.Get();
    init_info.NumFramesInFlight = 3;
    init_info.RenderTargetFormat = (WGPUTextureFormat) preferredFormat;
    init_info.DepthStencilFormat = (WGPUTextureFormat) depthTextureFormat;;
    ImGui_ImplWGPU_Init(&init_info);

#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = nullptr;
#endif
}

// Main code
int main(int, char**)
{
#if !defined(__EMSCRIPTEN__)
    #if defined(__linux__)
    #warning "LINUX USER: Please read here..."
    // it's necessary to specify  X11 or Wayland: use "wayland" instead of "x11" or ... default x11: it works also in wayland
    SDL_SetHint(SDL_HINT_VIDEODRIVER, "x11");  // or (outside code) export SDL_VIDEODRIVER=wayland environment variable
    #endif                                     // or    "      "    export SDL_VIDEODRIVER=$XDG_SESSION_TYPE to get the current session type
#endif
    // Init SDL
    SDL_Init(SDL_INIT_VIDEO);
    fwWindow = SDL_CreateWindow(appTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, initialWindowWidth, initialWindowHeight, SDL_WINDOW_RESIZABLE);

    initWGPU();


    // Initialize App Render Data
    initVGizmo3D();
    setScene();

    initRenderPipeline();
    initImGui();

    resizeSurface(initialWindowWidth, initialWindowHeight);


#ifdef __EMSCRIPTEN__
    // Main loop
    emscripten_set_main_loop([] {
        static SDL_Event event;
        while (SDL_PollEvent(&event)) ImGui_ImplSDL2_ProcessEvent(&event);
        mainLoop();
    }, 0, false);
#else
    SDL_Event event;
    bool canCloseWindow = false;
    // Main loop
    while (!canCloseWindow) {
        while (SDL_PollEvent(&event)) // Poll and handle events (inputs, window resize, etc.)
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT ||
               (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(fwWindow)))
                canCloseWindow = true;
        }
        mainLoop();
    }

    // Cleanup
    ImGui_ImplWGPU_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
#endif
    // All class destructors release the own object
    SDL_DestroyWindow(fwWindow);
    SDL_Quit();
    
    return 0;
}