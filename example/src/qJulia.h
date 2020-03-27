//------------------------------------------------------------------------------
//  Copyright (c) 2018-2019 Michele Morrone
//  All rights reserved.
//
//  https://michelemorrone.eu - https://BrutPitt.com
//
//  twitter: https://twitter.com/BrutPitt - github: https://github.com/BrutPitt
//
//  mailto:brutpitt@gmail.com - mailto:me@michelemorrone.eu
//  
//  This software is distributed under the terms of the BSD 2-Clause license
//------------------------------------------------------------------------------
#pragma once

#include <vGizmo.h>

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
        glBufferData(GL_ARRAY_BUFFER,size, vtx, GL_STATIC_DRAW);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vaoBuffer);
        glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, 0L);
        glEnableVertexAttribArray(vPosition);
        CHECK_GL_ERROR();
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

        CHECK_GL_ERROR();
}
private:
    GLuint vao, vaoBuffer;
    enum Attrib_IDs { vPosition = 0 };
    int size;
};



class qJulia : public mainProgramObj
{
public:

    qJulia() { initShaders(); }

    void initShaders();
    void render();

    vec4 quatPt = vec4(-0.65f, 0.4f, 0.25f, 0.05f);
    vec3 diffuseColor = vec3(0.3f,0.9f,0.65f);

    vec3 Light =  vec3(3.f,3.f,3.f);

    float phongMethod           = 1.0f ;
    float specularExponent      = 15.f;
    float specularComponent     = .5f  ;
    float normalComponent       = .25f ;
    float epsilon               = 0.001f;

    bool isFullRender = false;
    bool useShadow    = true;
    bool useAO        = false;

    mat3 matOrientation = mat3(1.0f);
    vec3 position = vec3(0.f);

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
        _position           ,
        _epsilon            ;

};