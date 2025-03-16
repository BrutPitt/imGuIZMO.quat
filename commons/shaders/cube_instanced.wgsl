//------------------------------------------------------------------------------
//  Copyright (c) 2025 Michele Morrone
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
R"(

    struct ubo {
        mvp     : mat4x4f,
        light   : mat4x4f,
    };

    // Model View Projection matrix.
    @group(0) @binding(0) var<uniform> u : ubo;

    struct vertIn
    {
        @location(0) pos: vec4f,
        @location(1) color: vec4f,
        @builtin(instance_index) instanceID : u32,
    };

    struct vertOut
    {
        @builtin(position) pos: vec4f,
        @location(0) color: vec4f,
    };

    struct fragIn
    {
        @location(0) color: vec4f
    };


    @vertex fn vs(in: vertIn) -> vertOut
    {
        var out: vertOut;
        if(in.instanceID == 1) {
            out.pos =  u.mvp * in.pos;
            out.color = in.color;
        } else {
            out.pos = u.light * in.pos;
            out.color = vec4f(1.0, 1.0, 0.5, 1.0);
        }

        return out;
    }

    @fragment fn fs(in: fragIn) -> @location(0) vec4f {
        return in.color;
    }
)"