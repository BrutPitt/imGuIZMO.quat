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

#include <cfloat>
#include <cstdlib>
#include <iostream>
#include <fstream>

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #include <emscripten/html5.h>
    #include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// ImGuIZMOquat / vGizmo3D:
#include <imguizmo_quat.h>

#include <imgui/backends/imgui_impl_opengl3.h>


void renderWidgets(vg::vGizmo3D &track, vec3& vLight, int width, int height);

class uniformBlocksClass {
public:

     uniformBlocksClass() { }
    ~uniformBlocksClass() { glDeleteBuffers(1, &uBuffer); }

// getting aligment for min size block allocation
    void getAlignment() {
        GLint uBufferMinSize(0);
        glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uBufferMinSize);
        uBlockSize = (GLint(realDataSize)/ uBufferMinSize) * uBufferMinSize;
        if(realDataSize%uBufferMinSize) uBlockSize += uBufferMinSize;
    }

    static GLuint bindIndex(GLuint prog, const char *nameUBlock, GLuint idx)
    {
        GLuint blockIndex = glGetUniformBlockIndex(prog, nameUBlock);
        glUniformBlockBinding(prog, blockIndex, idx);

        return blockIndex;
    }

#ifdef GLAPP_REQUIRE_OGL45
    void create(GLuint size, void *pData, GLuint idx = GLuint(bind::bindIdx))
#else
    void create(GLuint size, void *pData, GLuint prog, const char *nameUBlock, GLuint idx)
#endif
    {
        bindingLocation = idx;
        realDataSize = size;
        ptrData = pData;

        getAlignment();

#ifdef GLAPP_REQUIRE_OGL45
        glCreateBuffers(1, &uBuffer);
        glNamedBufferStorage(uBuffer,  uBlockSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
        if(ptrData) glNamedBufferSubData(uBuffer, 0, realDataSize, ptrData);
#else
        glGenBuffers(1,    &uBuffer);
        GLuint blockIndex = bindIndex(prog, nameUBlock, bindingLocation);
//get min size block sending
        GLint minBlockSize;
        glGetActiveUniformBlockiv(prog, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &minBlockSize);

        glBindBuffer(GL_UNIFORM_BUFFER,uBuffer);
// now we alloc min size permitted, but copy realDataSize
        glBufferData(GL_UNIFORM_BUFFER,  uBlockSize < minBlockSize ? minBlockSize : uBlockSize, nullptr, GL_DYNAMIC_DRAW);
        if(ptrData) glBufferSubData(GL_UNIFORM_BUFFER, 0, realDataSize, ptrData);
#endif
    }

    void updateBufferData(void *data=nullptr) {
#ifdef GLAPP_REQUIRE_OGL45
        glNamedBufferSubData(uBuffer, 0, realDataSize, data ? data : ptrData);
#else
        glBindBuffer(GL_UNIFORM_BUFFER,uBuffer);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, realDataSize, data ? data : ptrData);
#endif
        glBindBufferBase(GL_UNIFORM_BUFFER, bindingLocation, uBuffer);
    }


private:
    void *ptrData;
    GLuint bindingLocation;
    GLuint uBuffer;
    GLuint realDataSize, uBlockSize;
};


struct readShaderFile {
    readShaderFile(const std::string& fileName) {
        std::ifstream input(fileName,std::ifstream::binary);
            // Check to see that the file is open
        if (!input.is_open()) {
            std::cout << "ERROR: " << fileName << std::endl;
            throw std::runtime_error("failed to open file");
        }

        input.seekg(0, std::ios::end);
        const size_t size = input.tellg();
        input.seekg(0, std::ios::beg);

        // Allocate memory:
        str.resize(size, ' ');
        input.read(&str[0],  size);
        input.close();

        buffer = str.c_str();
    }

const char *buffer;
std::string str;
};

#define WGL_VERSION "#version 300 es\n"

const char* wgl_vert = WGL_VERSION R"(
precision highp float;
precision highp int;

layout(std140) uniform;

uniform uBuffer
{
    mat4 pMat;
    mat4 vMat;
    mat4 mMat;
    mat4 cMat;
    mat4 lMat;
    vec3 lightPos;
    vec3 PoV;
} u;

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 inColor;

out vec4 vsColor;
out vec4 vsNormal;
out vec4 vsPos;
out float shininessExp;

void main()
{
    if(gl_InstanceID==1) {       // cube
        vsColor = vec4(inColor, 1.0);
        vsNormal = vec4(mat3(u.mMat) * normal, 1.0);
        vsPos = u.mMat * vec4(pos, 1.0);

        gl_Position = u.pMat * u.vMat * u.cMat * vsPos;
        shininessExp = 500.f;   // BlinnPhong: use (about!) shininessExp*3 for similar Phong rendering apparence (view fragment)
    }
    else {                          // light
        vsNormal = vec4(mat3(u.lMat) * normal, 1.0);
        vsPos = u.lMat * vec4(pos, 1.0);
        vsColor = vec4(1.0, 1.0, 0.5, 1.0);        // light-cube have uniform color...
        shininessExp = 0.f;                         // and shininessExp have no sense
        gl_Position = u.pMat * u.vMat * u.cMat * u.lMat * vec4(pos*.1, 1.0);   // pos*.1 = reducing light-cube
    }
}
)";

const char* wgl_frag = WGL_VERSION R"(
precision highp float;
precision highp int;

uniform uBuffer
{
    mat4 pMat;
    mat4 vMat;
    mat4 mMat;
    mat4 cMat;
    mat4 lMat;
    vec3 lightPos;
    vec3 PoV;
} u;

in vec4 vsColor;
in vec4 vsNormal;
in vec4 vsPos;
in float shininessExp;

layout (location = 0) out vec4 outColor;

const float ambientInt   = 0.25;
const float specularInt  = 1.;

#define USE_BLINN_PHONG

void main()
{
        vec3 V = normalize(u.PoV.xyz + vsPos.xyz);
        vec3 L = normalize(u.lightPos.xyz - vsPos.xyz);
        vec3 N = normalize(vsNormal.xyz);

#ifdef USE_BLINN_PHONG
        vec3 H = normalize(L - V); // Half(angle)Vec
        float specular = pow(max(0.0, dot(H, N)), shininessExp);
#else // Phong
        vec3 R = reflect(L, N);
        float specular = pow(max(0.0, dot(R, V)), shininessExp);
#endif
        float diffuse = max(0.0, dot(N, L))*2.0;
        outColor = vec4(vsColor.rgb * (ambientInt + diffuse) + vec3(specular * specularInt), 1.0);
        outColor = min(vec4(1.0), outColor);

}
)";
