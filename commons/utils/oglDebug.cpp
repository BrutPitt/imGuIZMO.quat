//------------------------------------------------------------------------------
//  Copyright (c) 2018-2024 Michele Morrone
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
#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #include <emscripten/html5.h>
    #include <GLES3/gl3.h>
#else
    #include <cstdlib>
    #include <glad/glad.h>
#endif
#include <iostream>
#include <vector>
#include "oglDebug.h"

using namespace std;

void getCompilerLog(GLuint handle, GLint blen, bool isShader)
{
//if (!useGLSL) return aGLSLStrings[0];

    GLint len;
    GLchar* compilerLog;

    //if (ShaderObject==0) return aGLSLStrings[1]; // not a valid program object


    if (blen > 1) {
        compilerLog = new GLchar[blen];

        if(compilerLog!=NULL) {
            if(isShader) glGetShaderInfoLog( handle, blen, &len, compilerLog );
            else         glGetProgramInfoLog( handle, blen, &len, compilerLog );

            cout << compilerLog << "\n" << endl;

            delete compilerLog;
        } else cout << "Could not allocate InfoLog buffer\n" << endl;

        //cout << "compiler_log: \n", compiler_log);
    }
}

void checkShader(GLuint shader)
{
    GLint compiled;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );

#ifndef NDEBUG
    if(compiled == GL_FALSE) {
        GLint len;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH , &len);

        getCompilerLog(shader, len, true);
        exit(-1);
    }
#endif
}

void checkProgram(GLuint program)
{
    GLint linked;
    glGetProgramiv( program, GL_LINK_STATUS, &linked );
    //CHECK_GL_ERROR();

#ifndef NDEBUG
    if(linked == GL_FALSE) {
        GLint len;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH , &len);

        getCompilerLog(program, len, false);
        exit(-1);
    }
#endif
}

#define MAX_ERRORS_TO_SHOW 50
#ifndef __EMSCRIPTEN__
void enableDebugCallback() {
    glEnable(GL_DEBUG_OUTPUT);
    if(glDebugMessageCallback) {
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(openglCallbackFunction, nullptr);
        GLuint unusedIds = 0;
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, true);
    }
    else
        throw "glDebugMessageCallback not available: need OpenGL ES 3.2+ or OpenGL 4.3+";
}

void APIENTRY openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                       const GLchar* message, const void* userParam)
{
    static int count = 0;
    if(type!=GL_DEBUG_TYPE_OTHER /*&& count<MAX_ERRORS_TO_SHOW*/) {
        cout << endl << "----- debug message -----" << endl;
        cout << "message: "<< message << endl;
        cout << "type: ";
        switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            cout << "ERROR";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            cout << "DEPRECATED_BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            cout << "UNDEFINED_BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            cout << "PORTABILITY";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            cout << "PERFORMANCE";
            break;
        case GL_DEBUG_TYPE_OTHER:
            cout << "OTHER";
            break;
        }
        cout << endl;

        cout << "id: " << id << endl;
        cout << "severity: ";
        switch (severity){
        case GL_DEBUG_SEVERITY_LOW:
            cout << "LOW";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            cout << "MEDIUM";
            break;
        case GL_DEBUG_SEVERITY_HIGH:
            cout << "HIGH";
            break;
        default :
            cout << severity;
        }
        count++;
        cout << endl;
    }
}

void GetFirstNMessages(GLuint numMsgs)
{
    GLint maxMsgLen = 0;
    glGetIntegerv(GL_MAX_DEBUG_MESSAGE_LENGTH, &maxMsgLen);

    std::vector<GLchar> msgData(numMsgs * maxMsgLen);
    std::vector<GLenum> sources(numMsgs);
    std::vector<GLenum> types(numMsgs);
    std::vector<GLenum> severities(numMsgs);
    std::vector<GLuint> ids(numMsgs);
    std::vector<GLsizei> lengths(numMsgs);

    GLuint numFound = glGetDebugMessageLog(numMsgs, msgData.size(), &sources[0], &types[0], &ids[0], &severities[0], &lengths[0], &msgData[0]);

    sources.resize(numFound);
    types.resize(numFound);
    severities.resize(numFound);
    ids.resize(numFound);
    lengths.resize(numFound);

    std::vector<std::string> messages;
    messages.reserve(numFound);

    std::vector<GLchar>::iterator currPos = msgData.begin();
    for(size_t msg = 0; msg < lengths.size(); ++msg)
    {
       messages.push_back(std::string(currPos, currPos + lengths[msg] - 1));
       currPos = currPos + lengths[msg];
    }

    for(int i=0; i<numMsgs; i++)
        cout << "num: "<< numFound << " - src: " << sources[i] << " - type: " << types[i] << " - id: " << ids[i] << " - sev: " << severities[i] << endl << " ***** " << messages[i] << endl;
}
#endif

int CheckGLError(const char *file, int line)
{
    static int count = 0;

    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    while (glErr != GL_NO_ERROR)  {

        if(count++<MAX_ERRORS_TO_SHOW)
            cout << "GL error (" << (glErr) << ") " << " in File " << file << " at line: " << line << endl;
//        GetFirstNMessages(50);

        retCode = 1;
        glErr = glGetError();
    }
    return retCode;
}

