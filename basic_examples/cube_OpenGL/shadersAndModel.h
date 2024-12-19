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

#define GLSL_VERSION "#version 450\n"
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
const char* vertex_instanced = GLSL_VERSION R"(
layout(std140) uniform;
layout (location = 0) in vec4 pos;
layout (location = 1) in vec4 inColor;

layout (location = 2) uniform mat4 mvp;
layout (location = 3) uniform mat4 view;

layout (location = 0) out vec4 outColor;

out vec3 vsPos;

void main()
{
    if(gl_InstanceID==1) {
        outColor = inColor;
        gl_Position = mvp * (pos*.3);
    }
    else {
        outColor = vec4(1.0, 1.0, 0.5, 1.0);
        vec4 newCube = vec4((pos.xyz * .15), 1.0);
        gl_Position = view * newCube;
    }
}
)";
// fragment shader with (C)olor in and (C)olor out
const char* fragment_code = GLSL_VERSION R"(
layout (location = 0) in vec4 color;

layout (location = 0) out vec4 outColor;

const float near = .001;
const float far = 30.0;


void main()
{
    outColor = color;
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
  { -1.0f, -1.0f,  1.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f },
  { -1.0f,  1.0f,  1.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f },
  {  1.0f, -1.0f,  1.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f },
  {  1.0f, -1.0f,  1.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f },
  { -1.0f,  1.0f,  1.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f },
  {  1.0f,  1.0f,  1.0f, 1.0f,    1.0f, 0.0f, 0.0f, 1.0f },
  // green face
  { -1.0f, -1.0f, -1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f },
  {  1.0f, -1.0f, -1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f },
  { -1.0f,  1.0f, -1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f },
  { -1.0f,  1.0f, -1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f },
  {  1.0f, -1.0f, -1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f },
  {  1.0f,  1.0f, -1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 1.0f },
  // blue face
  { -1.0f,  1.0f,  1.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f },
  { -1.0f, -1.0f,  1.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f },
  { -1.0f,  1.0f, -1.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f },
  { -1.0f,  1.0f, -1.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f },
  { -1.0f, -1.0f,  1.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f },
  { -1.0f, -1.0f, -1.0f, 1.0f,    0.0f, 0.0f, 1.0f, 1.0f },
  // yellow face
  {  1.0f,  1.0f,  1.0f, 1.0f,    1.0f, 0.5f, 0.0f, 1.0f },
  {  1.0f,  1.0f, -1.0f, 1.0f,    1.0f, 0.5f, 0.0f, 1.0f },
  {  1.0f, -1.0f,  1.0f, 1.0f,    1.0f, 0.5f, 0.0f, 1.0f },
  {  1.0f, -1.0f,  1.0f, 1.0f,    1.0f, 0.5f, 0.0f, 1.0f },
  {  1.0f,  1.0f, -1.0f, 1.0f,    1.0f, 0.5f, 0.0f, 1.0f },
  {  1.0f, -1.0f, -1.0f, 1.0f,    1.0f, 0.5f, 0.0f, 1.0f },
  // magenta face
  {  1.0f,  1.0f,  1.0f, 1.0f,    1.0f, 0.0f, 1.0f, 1.0f },
  { -1.0f,  1.0f,  1.0f, 1.0f,    1.0f, 0.0f, 1.0f, 1.0f },
  {  1.0f,  1.0f, -1.0f, 1.0f,    1.0f, 0.0f, 1.0f, 1.0f },
  {  1.0f,  1.0f, -1.0f, 1.0f,    1.0f, 0.0f, 1.0f, 1.0f },
  { -1.0f,  1.0f,  1.0f, 1.0f,    1.0f, 0.0f, 1.0f, 1.0f },
  { -1.0f,  1.0f, -1.0f, 1.0f,    1.0f, 0.0f, 1.0f, 1.0f },
  // cyan face
  {  1.0f, -1.0f,  1.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f },
  {  1.0f, -1.0f, -1.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f },
  { -1.0f, -1.0f,  1.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f },
  { -1.0f, -1.0f,  1.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f },
  {  1.0f, -1.0f, -1.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f },
  { -1.0f, -1.0f, -1.0f, 1.0f,    0.0f, 1.0f, 1.0f, 1.0f },
};

