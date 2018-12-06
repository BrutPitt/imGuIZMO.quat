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
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "qJulia.h"

#include "glApp.h"

using namespace glm;


#define SHADER_PATH "Shaders/"



void qJulia::initShaders() 
{
    useVertex(); useFragment();

#ifdef __EMSCRIPTEN__
	getVertex()->Load("#version 300 es\n", 1, SHADER_PATH "qjVert.glsl");
	getFragment()->Load("#version 300 es\n", 1 ,SHADER_PATH "qjFragES2.glsl");
#else
    #ifdef GLAPP_NO_GLSL_PIPELINE
        getVertex()->Load("#version 430\n", 1 ,SHADER_PATH "qjVert.glsl");
    #else
        getVertex()->Load("#version 430\n#define QJSET_USE_PIPELINE\n", 1 ,SHADER_PATH "qjVert.glsl");
    #endif

	getFragment()->Load("#version 430\n", 1 ,SHADER_PATH "qjFragES2.glsl");
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
    
    reset();
}

void qJulia::render() 
{
    bindPipeline();
    useProgram();

    glUniform4fv(_quatPt            ,1  , glm::value_ptr(quatPt));
    glUniform3fv(_Resolution        ,1  , glm::value_ptr(vec3(theApp->GetWidth(), theApp->GetHeight(),float(theApp->GetWidth())/float(theApp->GetHeight()))));
    glUniform3fv(_diffuseColor      ,1  , glm::value_ptr(diffuseColor));
    glUniform3fv(_Light             ,1  , glm::value_ptr(Light));

    glUniform1f (_phongMethod       , phongMethod      );
    glUniform1f (_specularExponent  , specularExponent );
    glUniform1f (_specularComponent , specularComponent);
    glUniform1f (_normalComponent   , normalComponent  );
    glUniform1f (_epsilon           , epsilon           );

    glUniform1i (_isFullRender , isFullRender);
    glUniform1i (_useShadow    , useShadow   );
    glUniform1i (_useAO        , useAO       );

        
    glUniformMatrix3fv(_matOrientation, 1, GL_FALSE, glm::value_ptr(inverse(matOrientation)));

    //glBindVertexArray(vao);       
        
    vao.draw();


    reset();

}