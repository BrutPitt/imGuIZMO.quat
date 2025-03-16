//------------------------------------------------------------------------------
//  Copyright (c) 2018-2024 Michele Morrone
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
#line 13  


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