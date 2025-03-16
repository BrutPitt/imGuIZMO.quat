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

#include <glad/glad.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/backends/imgui_impl_opengl3.h>

/////////////////////////////////////////////////////////////////////////////
// ImGuIZMOquat / vGizmo3D:
#include <imguizmo_quat.h>

void renderWidgets(vg::vGizmo3D &track, vec3& vLight, int width, int height);

class uniformBlocksClass {
public:
     uniformBlocksClass() { }
    ~uniformBlocksClass() { glDeleteBuffers(1, &uBuffer); }

// get aligment for min size block allocation
    void getAlignment() {
        GLint uBufferMinSize(0);
        glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uBufferMinSize);
        uBlockSize = (GLint(realDataSize)/ uBufferMinSize) * uBufferMinSize;
        if(realDataSize%uBufferMinSize) uBlockSize += uBufferMinSize;
    }

    void create(GLuint size, void *pData, GLuint idx)  {
        bindingLocation = idx;
        realDataSize = size;
        ptrData = pData;

        getAlignment();

        glCreateBuffers(1, &uBuffer);
        glNamedBufferStorage(uBuffer,  uBlockSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
        if(ptrData) glNamedBufferSubData(uBuffer, 0, realDataSize, ptrData);
    }

    void updateBufferData(void *data=nullptr) {
        glNamedBufferSubData(uBuffer, 0, realDataSize, data ? data : ptrData);
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