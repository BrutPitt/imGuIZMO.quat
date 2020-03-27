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
//Added from loader
//#version 300 es
//#version 430 or 450

#ifdef GL_ES
precision highp float;
#endif

/*
uniform vec3 Eye = vec3(0, 0, 2.2);

uniform vec3 Light = vec3(3, 3, 3);
uniform vec3 LightColor = vec3(0.95, 0.7, 0.1);
uniform vec4 Quat = vec4(0.25, 0.5, 0.0, 0.25);
uniform float Epsilon = 0.003;
uniform float Slice=.1;
uniform int maxIterations = 10;
out vec4 Color;
*/
out vec4 outColor;
uniform vec3 resolution;
uniform vec4 quatPt;
uniform vec3  diffuseColor;
uniform float phongMethod;
uniform float specularExponent;
uniform float specularComponent;
uniform float normalComponent;
uniform bool isFullRender;
uniform bool useShadow;
uniform bool useAO;

uniform mat3 matOrientation;
uniform vec3 position;
uniform vec3 Light;
uniform float epsilon;
//uniform mat3 Orientation;
//uniform float qjR, qjG, qjB;
//uniform vec3 LightColor;
//uniform float zzzz;

const vec3 Eye = vec3(0, 0, 2.2);
//mat3 Orientation = mat3(1);
//mat3 Orientation = mat3(qjR,0.,0., qjB,qjG,0., 0.,0.,1.0);
//const vec3 Light = vec3(3., 3., 3.);
//const vec3 LightColor = vec3(0.95, 0.7, 0.1);
//const vec4 Quat = vec4(0.25, 0.5, 0.0, 0.25);
const float Slice=.0;
const int maxIterations = 10;

//bool isFullRender = true;

const float BOUNDING_RAD = 8.0;
const float ESCAPE_THRESHOLD = 1e1;
const float D_EPS = 1e-4;

vec4 QuatMul(vec4 q1, vec4 q2) {
    vec4 r;
    r.x = q1.x * q2.x - dot(q1.yzw, q2.yzw);
    r.yzw = q1.x * q2.yzw + q2.x * q1.yzw + cross(q1.yzw, q2.yzw);
    return r;
}

vec4 QuatSq(vec4 q) {
    vec4 r;
    r.x = q.x * q.x - dot(q.yzw, q.yzw);
    r.yzw = 2. * q.x * q.yzw;
    return r;
}

void IterateIntersect(inout vec4 z, inout vec4 zp, vec4 c, out vec4 trap) 
{
        trap = vec4(abs(z.xyz),dot(z,z));
		for (int h = 0; h < maxIterations; h++) {
			zp = 2.0 * QuatMul(z, zp);
			z = QuatSq(z) + c;
            trap = min( trap, vec4(abs(z.xyz),dot(z,z)) );
			if (dot(z, z) > ESCAPE_THRESHOLD) break;
		}
}

vec3 NormEstimate(vec3 p, vec4 c, float slice) {
    vec3 N;
    float dX, dY, dZ;
    vec4 g0 = vec4(p                    , slice);
    vec4 gx = vec4(p + vec3(D_EPS, 0, 0), slice);
    vec4 gy = vec4(p + vec3(0, D_EPS, 0), slice);
    vec4 gz = vec4(p + vec3(0, 0, D_EPS), slice);
    for (int i = 0; i < maxIterations; i++) {
        g0 = QuatSq(g0) + c;
        gx = QuatSq(gx) + c;
        gy = QuatSq(gy) + c;
        gz = QuatSq(gz) + c;
    }
    float ln = length(g0);
    dX = length(gx) - ln;
    dY = length(gy) - ln;
    dZ = length(gz) - ln;
    return normalize(vec3(dX, dY, dZ));
}


vec3 NormEstimateA(vec3 p, vec4 c, float slice) {
    vec3 N;
    float dX, dY, dZ;
    vec4 gx1 = vec4(p - vec3(D_EPS, 0, 0), slice);
    vec4 gx2 = vec4(p + vec3(D_EPS, 0, 0), slice);
    vec4 gy1 = vec4(p - vec3(0, D_EPS, 0), slice);
    vec4 gy2 = vec4(p + vec3(0, D_EPS, 0), slice);
    vec4 gz1 = vec4(p - vec3(0, 0, D_EPS), slice);
    vec4 gz2 = vec4(p + vec3(0, 0, D_EPS), slice);
    for (int i = 0; i < maxIterations; i++) {
        gx1 = QuatSq(gx1) + c;
        gx2 = QuatSq(gx2) + c;
        gy1 = QuatSq(gy1) + c;
        gy2 = QuatSq(gy2) + c;
        gz1 = QuatSq(gz1) + c;
        gz2 = QuatSq(gz2) + c;
    }
    dX = length(gx2) - length(gx1);
    dY = length(gy2) - length(gy1);
    dZ = length(gz2) - length(gz1);
    return normalize(vec3(dX, dY, dZ));
}


bool jinteresct(in vec3 rO, in vec3 rD, in vec4 c, out float resT, out vec4 resC )
{
    float mz2,md2,dist;
	vec4 z,nz;
	
    float t = 0.0;
	for( int j=0; j<220; j++ ) 	{
	    vec3 p = rO + t*rD;

		z = vec4(p,0.0);
		md2 = 1.0;
		mz2 = dot(z,z);

        vec4 trap = vec4(abs(z.xyz),dot(z,z));

		for(int i=0;i<12;i++) {			
			md2*=4.0*mz2;			
	        nz.x=z.x*z.x-dot(z.yzw,z.yzw); // z -> z2 + c
			nz.yzw=2.0*z.x*z.yzw;
			z=nz+c;

	        trap = min( trap, vec4(abs(z.xyz),dot(z,z)) );

			mz2 = dot(z,z);
			if(mz2>4.0) break;
		}

		dist = 0.25*sqrt(mz2/md2)*log(mz2);

		if(dist<epsilon) {
            resC = trap;
			resT = t;
			return true;
		}
		
		t += dist;
	}

	return false;
}


float IntersectQJulia(inout vec3 rO, inout vec3 rD, vec4 c, float slice, out float AO, out vec4 trap) {
    float dist;
    float ao =0.0;
    for(int i = 0; i<250; i++) {
        vec4 z = vec4(rO, slice);
        vec4 zp = vec4(1., 0., 0., 0.);
        IterateIntersect(z, zp, c, trap);
        float normZ = length(z);      
        dist = 0.5 * normZ * log(normZ) / length(zp);
        rO += rD * dist;
        ao += dist;
        if (dist < epsilon || dot(rO, rO) > BOUNDING_RAD) break;
    }
    AO = ao;
    return dist;
}

vec3 Phong(vec3 light, vec3 eye, vec3 pt, vec3 N) {
    vec3 diffuse = diffuseColor;
    //float specularExponent = .1;
    //float specularity = .5;
    vec3 L = normalize(light - pt);
    vec3 E = normalize(eye - pt);
    float NdotL = dot(N, L);
    vec3 R = L - 2. * NdotL * N;
    diffuse += abs(N) * normalComponent;
    return vec3(0.1) + diffuse * max(NdotL, 0.0) + specularComponent * pow(max(dot(E, R), 0.0), specularExponent);
}


#define JI

#define NR 1.0

vec3 sampleCoord[9];


void main() {
    vec4 Quat = quatPt;

	mat3 orient = matOrientation;
	 sampleCoord[0] = vec3( 0.,  0., 1.);
	 sampleCoord[1] = vec3( NR, -NR, .3);
	 sampleCoord[2] = vec3(-NR,  NR, .3);
	 sampleCoord[3] = vec3( NR,  NR, .3);
	 sampleCoord[4] = vec3(-NR, -NR, .3);
	 sampleCoord[5] = vec3(  0,  NR, .5);
	 sampleCoord[6] = vec3(  0, -NR, .5);
	 sampleCoord[7] = vec3( NR,   0, .5);
	 sampleCoord[8] = vec3(-NR,   0, .5);

	//float colorDiv = 1., shadowDiv = 1.0, colorShadow = 1.;
    float colorDiv = 4., shadowDiv = 9.0, colorShadow = 0.;
    vec3 color = vec3(0.);
    for (int i = 0; i < 9; i++) {
		//vec2 coord = isFullRender ? gl_FragCoord.xy*2. + sampleCoord[i] : gl_FragCoord.xy*2.;

        //vec2 coord = (gl_FragCoord.xy*2. + sampleCoord[i].xy) / resolution.xy - vec2(1.);

        vec2 coord = (gl_FragCoord.xy*2. + sampleCoord[i].xy) / resolution.xy - vec2(1.);

        vec3 ray = vec3(coord.x * resolution.z, coord.y, -1.);
        vec3 dir = orient * ray + position;
        vec3 origin = orient * Eye + position;
        vec4 col;
        float AO, shadow = 1.0;

#ifdef JI
        float dist = IntersectQJulia(origin, dir, Quat, Slice, AO, col);
        if (dist < epsilon) {
            vec3 N = NormEstimate(origin, Quat, Slice);

#else
		float dist;
		
        if (jinteresct(origin, dir, Quat, dist,col)) {
            vec3 N = NormEstimate(origin+dist*dir, Quat, Slice);
            origin += dist*dir;
#endif                  
            float aoComponent = useAO ? (.5+.5*clamp( col.w*1.5 , .0, 1.0 )) : 1.0;
            color += Phong(orient * (Light*phongMethod), orient * Eye, origin, N*phongMethod) * aoComponent * sampleCoord[i].z;

            if(useShadow) {
                vec3 L = normalize( orient*Light - origin );
                origin += N*epsilon*2.0;
                dist = IntersectQJulia( origin, L, Quat, Slice, AO, col );

                 // Again, if our estimate of the distance to the set is small, we say
                 // that there was a hit.  In this case it means that the point is in
                 // shadow and should be given darker color.
                 if( dist < epsilon ) colorShadow += 0.5;  // (darkening the shaded value is not really correct, but looks good)
                 else                 colorShadow += 1.0;
            }

        }
    	if(!isFullRender) { colorDiv = 1.; shadowDiv = 1.; break; } // fast and approximative render, but looks alredy good 

    }
    outColor = vec4(color/colorDiv, 1.);
    if(useShadow) outColor.xyz *= colorShadow/shadowDiv;

}
