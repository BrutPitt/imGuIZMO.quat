///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2018 Michele Morrone
//  All rights reserved.
//
//  mailto:me@michelemorrone.eu
//  mailto:brutpitt@gmail.com
//  
//  https://github.com/BrutPitt
//
//  https://michelemorrone.eu
//  
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the <organization> nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//  
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glslProgramObject.h>

class vaoClass {
public:
    vaoClass() {
        float vtx[] = {-1.0f,-1.0f,
                        1.0f,-1.0f,
                        1.0f, 1.0f,
                       -1.0f, 1.0f };


        size = sizeof(vtx);
#ifdef GLAPP_REQUIRE_OGL45
        glCreateVertexArrays(1, &vao);
        glCreateBuffers(1, &vaoBuffer);
        glNamedBufferStorage(vaoBuffer, size, vtx, 0); 

        glVertexArrayAttribBinding(vao,vPosition, 0);
        glVertexArrayAttribFormat(vao, vPosition, 2, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayVertexBuffer(vao, 0, vaoBuffer, 0, 8);

        glEnableVertexArrayAttrib(vao, vPosition);        

#else
        glGenVertexArrays(1, &vao); 
        glGenBuffers(1, &vaoBuffer);
        glBindBuffer(GL_ARRAY_BUFFER,vaoBuffer);
        glBufferData(GL_ARRAY_BUFFER,size, vtx, GL_STATIC_READ);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vaoBuffer);
        glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, 0L);
        glEnableVertexAttribArray(vPosition);
#endif
    }

    ~vaoClass() {
        glDeleteBuffers(1, &vaoBuffer);
        glDeleteVertexArrays(1, &vao);
    }

    void enable() {        
      

    }

    void draw() {
        glBindVertexArray(vao);        
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    }
private:
    GLuint vao, vaoBuffer;
    enum Attrib_IDs { vPosition = 2 };
    int size;
};



class qJulia : public mainProgramObj
{
public:

    qJulia() { initShaders(); }

    void initShaders();
    void render();

    glm::vec4 quatPt = glm::vec4(-0.65, 0.4, 0.25, 0.05);
    glm::vec3 diffuseColor = glm::vec3(0.3,0.9,0.65);

    glm::vec3 Light =  glm::vec3(3.f,3.f,3.f);

    float phongMethod           = 1.0 ;
    float specularExponent      = 15.f;
    float specularComponent     = .5  ;
    float normalComponent       = .25 ;
    float epsilon               = 0.001;

    bool isFullRender = false;
    bool useShadow    = true;
    bool useAO        = false;

    glm::mat3 matOrientation = glm::mat3(1.0);

    vaoClass vao;

GLuint  _Resolution         ,
        _quatPt             ,
        _diffuseColor       ,
        _phongMethod        ,
        _specularExponent   ,
        _specularComponent  ,
        _normalComponent    ,
        _isFullRender       ,
        _useShadow          ,
        _useAO              ,
        _matOrientation     ,
        _Light              ,
        _epsilon            ;

};