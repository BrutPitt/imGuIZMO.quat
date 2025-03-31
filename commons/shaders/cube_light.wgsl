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
        pMat   : mat4x4f,
        vMat   : mat4x4f,
        mMat   : mat4x4f,
        cMat   : mat4x4f,
        lMat   : mat4x4f,
        lightP : vec3f,
        PoV    : vec3f,
    };

    // Model View Projection matrix.
    @group(0) @binding(0) var<uniform> u : ubo;

    struct vertIn
    {
        @location(0) pos: vec4f,
        @location(1) normal: vec4f,
        @location(2) color: vec4f,
        @builtin(instance_index) instanceID : u32,
    };

    struct vertOut
    {
        @builtin(position) gPos: vec4f,
        @location(0) color: vec4f,
        @location(1) normal: vec4f,
        @location(2) pos: vec4f,
        @location(3) shininessExp: f32,
    };

    struct fragIn
    {
        @location(0) color: vec4f,
        @location(1) normal: vec4f,
        @location(2) pos: vec4f,
        @location(3) shininessExp: f32,
    };

    @vertex fn vs(in: vertIn) -> vertOut
    {
        var out: vertOut;
        if(in.instanceID == 1) {
            out.color = in.color;
            let m3 = mat3x3(u.mMat[0].xyz, u.mMat[1].xyz, u.mMat[2].xyz);           // there is no constructor to build mat3x3 from mat4x4
            out.normal = vec4f(m3 * in.normal.xyz, 1.0);                            // so or I use this to build a mat3x3... or ...
            // out.normal = vec4f((u.mMat * in.normal).xyz, 1.0);                   // ... use this with overload of unused operations on mat4x4
            out.pos =  u.mMat * in.pos;
            out.shininessExp = 500.f;   // BlinnPhong: use (about!) shininessExp*3 for similar Phong rendering apparence (view fragment)

            out.gPos = u.pMat * u.vMat * u.cMat * out.pos;
        } else {
            out.color = vec4f(1.0, 1.0, 0.5, 1.0);        // light-cube have uniform color...
            let m3 = mat3x3(u.lMat[0].xyz, u.lMat[1].xyz, u.lMat[2].xyz);           // there is no constructor to build mat3x3 from mat4x4
            out.normal = vec4f(m3 * in.normal.xyz, 1.0);                            // so or I use this to build a mat3x3... or ...
            // out.normal = vec4f((u.lMat * in.normal).xyz, 1.0);                   // ... use this with overload of unused operations on mat4x4
            out.pos =  u.lMat * in.pos;

            out.shininessExp = 0.f;                         // and shininessExp have no sense
            out.gPos = u.pMat * u.vMat * u.cMat * u.lMat * (in.pos*.1);   // pos*.1 = reducing light-cube
        }

        return out;
    }

    @fragment fn fs(in: fragIn) -> @location(0) vec4f {

        let ambientInt  : f32 = 0.25;
        let specularInt : f32 = 1.;

        let V : vec3f = normalize(u.PoV + in.pos.xyz);
        let L : vec3f = normalize(u.lightP - in.pos.xyz);
        let N : vec3f = normalize(in.normal.xyz);

        let H : vec3f = normalize(L - V); // Half(angle)Vec
        let specular : f32 = pow(max(0.0, dot(H, N)), in.shininessExp);

        let diffuse : f32 = max(0.0, dot(N, L))*2.0;
        let outColor : vec4f = vec4f(in.color.rgb * (ambientInt + diffuse) + vec3f(specular * specularInt), 1.0);
        return  min(vec4f(1.0), outColor);
    }
)"