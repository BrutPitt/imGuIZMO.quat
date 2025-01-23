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

#define GLSL_VERSION "#version 450\n"

// OpenGL - Sample cube
const char* vertex_code = GLSL_VERSION R"(
layout(std140) uniform;
layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 inColor;

layout (location = 2) uniform mat4 mvp;

layout (location = 0) out vec4 outColor;

out vec3 vsPos;


void main()
{
    outColor = inColor;
    gl_Position = mvp * pos;
}
)";

// OpenGL - Sample cube instanced (to simulate light spot)
const char* vertex_instanced = GLSL_VERSION R"(
layout(std140) uniform;
layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 inColor;

layout (location = 2) uniform mat4 mvp;
layout (location = 3) uniform mat4 light;

layout (location = 0) out vec4 outColor;

void main()
{
    if(gl_InstanceID==1) {
        outColor = inColor;
        gl_Position = mvp * pos;
    }
    else {
        outColor = vec4(1.0, 1.0, 0.5, 1.0);
        gl_Position = light * pos ;
    }
}
)";

// Vulkan - Sample cube
const char* vk_vertex_code = GLSL_VERSION R"(
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

struct matrices
{
  mat4 mvp;
  mat4 light;
};

layout (std140, binding = 0) uniform uBuffer
{
    matrices uMat;
};

layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 inColor;

layout (location = 0) out vec4 outColor;

void main()
{
  outColor = inColor;
  gl_Position = uMat.mvp * pos;
}
)";


// Vulkan - Sample cube instanced (to simulate light spot)
const char* vk_vertex_instanced = GLSL_VERSION  R"(
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

struct matrices
{
  mat4 mvp;
  mat4 light;
};

layout (std140, binding = 0) uniform uBuffer
{
    matrices uMat;
};

layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 inColor;

layout (location = 0) out vec4 outColor;

void main()
{
    if(gl_InstanceIndex==1) {
        outColor = inColor;
        gl_Position = uMat.mvp * pos;
    }
    else {
        outColor = vec4(1.0, 1.0, 0.5, 1.0);
        gl_Position = uMat.light * pos;
    }
}
)";

// fragment shader with (C)olor in and (C)olor out
const char* fragment_code = GLSL_VERSION R"(
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec4 color;
layout (location = 0) out vec4 outColor;

void main()
{
    outColor = color;
}
)";

const char* vk_vert_inv_z = GLSL_VERSION  R"(
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

struct matrices
{
  mat4 mvp;
  mat4 light;
};

layout (std140, binding = 0) uniform uBuffer
{
    matrices uMat;
};

layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 inColor;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 vPos;

void main()
{
    if(gl_InstanceIndex==1) {
        outColor = inColor;
        gl_Position = uMat.mvp * pos;
    }
    else {
        outColor = vec4(1.0, 1.0, 0.5, 1.0);
        gl_Position = uMat.light * pos;
    }
    vPos = gl_Position;
}
)";

const char* vk_frag_inv_z = GLSL_VERSION R"(
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec4 color;
layout (location = 1) in vec4 vPos;

layout (location = 0) out vec4 outColor;

// far & near values are the same of perspective() call and can be passed to shader
float far = 100.0;  // copied here for simplicity
float near = .1;

float linearizeDepth(float d) // draw in zNDC [0, 1]
{
    return near * far / (far + d * (near - far));
}

void main()
{
    outColor = color;
    // invert depth value
    gl_FragDepth = 1.0 - linearizeDepth(vPos.z);
}
)";

struct VertexPC
{
  float x, y, z, w;   // Position
  float r, g, b, a;   // Color
};

static const VertexPC coloredCubeData[] =
{
  // red face
  { -1.0f, -1.0f,  1.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f },
  { -1.0f,  1.0f,  1.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f },
  {  1.0f, -1.0f,  1.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f },
  {  1.0f, -1.0f,  1.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f },
  { -1.0f,  1.0f,  1.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f },
  {  1.0f,  1.0f,  1.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f },
  // green face
  { -1.0f, -1.0f, -1.0f, 1.0f,    0.7f, 0.7f, 1.0f, 1.0f },
  {  1.0f, -1.0f, -1.0f, 1.0f,    0.7f, 0.7f, 1.0f, 1.0f },
  { -1.0f,  1.0f, -1.0f, 1.0f,    0.7f, 0.7f, 1.0f, 1.0f },
  { -1.0f,  1.0f, -1.0f, 1.0f,    0.7f, 0.7f, 1.0f, 1.0f },
  {  1.0f, -1.0f, -1.0f, 1.0f,    0.7f, 0.7f, 1.0f, 1.0f },
  {  1.0f,  1.0f, -1.0f, 1.0f,    0.7f, 0.7f, 1.0f, 1.0f },
  // blue face
  { -1.0f,  1.0f,  1.0f, 1.0f,    1.0f, 0.7f, 0.7f, 1.0f },
  { -1.0f, -1.0f,  1.0f, 1.0f,    1.0f, 0.7f, 0.7f, 1.0f },
  { -1.0f,  1.0f, -1.0f, 1.0f,    1.0f, 0.7f, 0.7f, 1.0f },
  { -1.0f,  1.0f, -1.0f, 1.0f,    1.0f, 0.7f, 0.7f, 1.0f },
  { -1.0f, -1.0f,  1.0f, 1.0f,    1.0f, 0.7f, 0.7f, 1.0f },
  { -1.0f, -1.0f, -1.0f, 1.0f,    1.0f, 0.7f, 0.7f, 1.0f },
  // yellow face
  {  1.0f,  1.0f,  1.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f },
  {  1.0f,  1.0f, -1.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f },
  {  1.0f, -1.0f,  1.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f },
  {  1.0f, -1.0f,  1.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f },
  {  1.0f,  1.0f, -1.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f },
  {  1.0f, -1.0f, -1.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f },
  // magenta face
  {  1.0f,  1.0f,  1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f },
  { -1.0f,  1.0f,  1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f },
  {  1.0f,  1.0f, -1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f },
  {  1.0f,  1.0f, -1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f },
  { -1.0f,  1.0f,  1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f },
  { -1.0f,  1.0f, -1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f },
  // cyan face
  {  1.0f, -1.0f,  1.0f, 1.0f,    0.7f, 1.0f, 0.7f, 1.0f },
  {  1.0f, -1.0f, -1.0f, 1.0f,    0.7f, 1.0f, 0.7f, 1.0f },
  { -1.0f, -1.0f,  1.0f, 1.0f,    0.7f, 1.0f, 0.7f, 1.0f },
  { -1.0f, -1.0f,  1.0f, 1.0f,    0.7f, 1.0f, 0.7f, 1.0f },
  {  1.0f, -1.0f, -1.0f, 1.0f,    0.7f, 1.0f, 0.7f, 1.0f },
  { -1.0f, -1.0f, -1.0f, 1.0f,    0.7f, 1.0f, 0.7f, 1.0f },
};

#define WGL_VERSION "#version 300 es\n"

const char* wgl_vertex_instanced = WGL_VERSION R"(
precision highp float;
precision highp int;

layout(std140) uniform;
layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 inColor;

uniform mat4 mvp;
uniform mat4 light;

out vec4 vtxColor;

void main()
{
    if(gl_InstanceID==1) {
        vtxColor = inColor;
        gl_Position = mvp * pos;
    }
    else {
        vtxColor = vec4(1.0, 1.0, 0.5, 1.0);
        gl_Position = light * pos ;
    }
}
)";

const char* wgl_fragment_code = WGL_VERSION R"(
precision highp float;
precision highp int;

in vec4 vtxColor;
out vec4 outColor;

void main()
{
    outColor = vtxColor;
}
)";
