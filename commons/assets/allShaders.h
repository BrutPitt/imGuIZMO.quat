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

// Vulkan - Sample cube instanced (to simulate light spot)
const char* vk_vertex_inst_light = GLSL_VERSION  R"(
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

struct matrices
{
        mat4 pMat;
        mat4 vMat;
        mat4 mMat;
        mat4 cMat;
        mat4 tMat;
        mat4 lMat;
        vec3 lightPos;
};

layout (std140, binding = 0) uniform uBuffer
{
    matrices u;
};

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 inColor;

layout (location = 0) out vsOut {
    vec4 outColor;
    vec4 vsNormal;
    vec4 vsPos;
    vec4 vsLight;
    float shin;
    vec3 PoV;
};

void main()
{
    vsLight = u.tMat * u.lMat * vec4(1.0); //vec4(u.lightPos, 1.0);
    //vsLight = u.tMat * vec4(u.lightPos, 1.0);
    PoV = (u.vMat * vec4(1.0)).xyz;      //vec4(pos,1)

    if(gl_InstanceIndex==1) {
        outColor = vec4(inColor, 1.0);
        vsNormal = u.mMat * vec4(normal, 1.0);
        vsPos = u.tMat * u.mMat * vec4(pos, 1.0);

        gl_Position = u.pMat * u.vMat * u.cMat * vsPos;
        shin = 500.f;
    }
    else {
        shin = 0.f;
        vsNormal = u.lMat * vec4(normal, 1.0);
        vsPos = u.tMat * u.lMat * vec4(pos, 1.0);
        outColor = vec4(1.0, 1.0, 0.5, 1.0);
        gl_Position = u.pMat * u.vMat * u.cMat * u.tMat * u.lMat * vec4(pos, 1.0);
    }
}
)";
// fragment shader with (C)olor in and (C)olor out
const char* fragment_code_light = GLSL_VERSION R"(
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable


layout (location = 0) in vsOut {
    vec4 vsColor;
    vec4 vsNormal;
    vec4 vsPos;
    vec4 vsLight;
    float shin;
    vec3 PoV;
};

layout (location = 0) out vec4 outColor;

const float ambientInt   = 0.25;
const float specularInt  = 1.;
const float shininessExp = 10.f;

void main()
{
        vec3 eyeDirection   = normalize(PoV+vsPos.xyz);
        vec3 lightDir       = normalize(vsLight.xyz-vsPos.xyz);
        vec3 normal         = normalize(vsNormal.xyz);

        vec3 halfVec = normalize(lightDir - eyeDirection);
        vec3 R = reflect(-lightDir, normal);

        float diffuse = max(0.0, dot(normal, lightDir))*2.0;
        float specular = pow(max(0.0, dot(halfVec, normal)), shin);
        outColor = vsColor * ambientInt + vsColor * diffuse + vec4(specular * specularInt) /*- distAtten*/;
        outColor = min(vec4(1.0), outColor);

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
