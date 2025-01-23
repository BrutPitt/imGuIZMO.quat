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
#pragma once
void getCompilerLog(GLuint handle, GLint blen, bool isShader);
void checkShader(GLuint shader);
void checkProgram(GLuint program);
int CheckGLError(const char *file, int line);

#ifndef __EMSCRIPTEN__
void GetFirstNMessages(GLuint numMsgs);
void APIENTRY openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
void enableDebugCallback();
#endif

#if !defined(NDEBUG)
#define CHECK_GL_ERROR()      CheckGLError(__FILE__, __LINE__);
#define CHECK_GL_ERROR_MSG(X) printf(X); CheckGLError(__FILE__, __LINE__);
#else
#define CHECK_GL_ERROR()
#define CHECK_GL_ERROR_MSG(X)
#endif
