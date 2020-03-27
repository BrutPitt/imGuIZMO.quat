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
#include "qJulia.h"

#include "glApp.h"

//using namespace glm;


#define SHADER_PATH "Shaders/"



void qJulia::initShaders() 
{
    useVertex(); useFragment();

#ifdef __EMSCRIPTEN__
	getVertex()->Load("#version 300 es\n", 1, SHADER_PATH "qjVert.glsl");
	getFragment()->Load("#version 300 es\n", 1 ,SHADER_PATH "qjFragES2.glsl");
#else
    #ifdef GLAPP_NO_GLSL_PIPELINE
        getVertex()->Load("#version 410\n", 1 ,SHADER_PATH "qjVert.glsl");
    #else
        getVertex()->Load("#version 410\n#define QJSET_USE_PIPELINE\n", 1 ,SHADER_PATH "qjVert.glsl");
    #endif

	getFragment()->Load("#version 410\n", 1 ,SHADER_PATH "qjFragES2.glsl");
#endif
	// The vertex and fragment are added to the program object
    addVertex();
    addFragment();

	link();

    bindPipeline();
    useProgram();

    _Resolution         = getUniformLocation("resolution");
    _quatPt             = getUniformLocation("quatPt");
    _diffuseColor       = getUniformLocation("diffuseColor");
    _phongMethod        = getUniformLocation("phongMethod");
    _specularExponent   = getUniformLocation("specularExponent");
    _specularComponent  = getUniformLocation("specularComponent");
    _normalComponent    = getUniformLocation("normalComponent");
    _isFullRender       = getUniformLocation("isFullRender");
    _useShadow          = getUniformLocation("useShadow");
    _useAO              = getUniformLocation("useAO");
    _matOrientation     = getUniformLocation("matOrientation");
    _Light              = getUniformLocation("Light");
    _epsilon            = getUniformLocation("epsilon");
    _position           = getUniformLocation("position");
    
    reset();
}

void qJulia::render() 
{
    bindPipeline();
    useProgram();

    glUniform4fv(_quatPt            ,1  , value_ptr(quatPt));
    glUniform3fv(_Resolution        ,1  , value_ptr(vec3(theApp->GetWidth(), theApp->GetHeight(),float(theApp->GetWidth())/float(theApp->GetHeight()))));
    glUniform3fv(_diffuseColor      ,1  , value_ptr(diffuseColor));
    glUniform3fv(_Light             ,1  , value_ptr(Light));

    glUniform1f (_phongMethod       , phongMethod      );
    glUniform1f (_specularExponent  , specularExponent );
    glUniform1f (_specularComponent , specularComponent);
    glUniform1f (_normalComponent   , normalComponent  );
    glUniform1f (_epsilon           , epsilon           );

    glUniform1i (_isFullRender , isFullRender);
    glUniform1i (_useShadow    , useShadow   );
    glUniform1i (_useAO        , useAO       );

    glUniform3fv(_position, 1, value_ptr(-position));
        
    glUniformMatrix3fv(_matOrientation, 1, GL_FALSE, value_ptr(transpose(matOrientation)));

    //glBindVertexArray(vao);       
        
    vao.draw();
    CHECK_GL_ERROR();

    reset();

}