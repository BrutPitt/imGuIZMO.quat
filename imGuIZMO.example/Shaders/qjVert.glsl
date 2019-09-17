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
#line 12

//#version 430 core
layout (location = 0) in vec2 vPos;

#ifdef QJSET_USE_PIPELINE
out gl_PerVertex
{
	vec4 gl_Position;
};
#endif

void main(void)
{
    gl_Position = vec4(vPos.xy,.0f,1.f);
}